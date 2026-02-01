#!/usr/bin/env bash
set -euo pipefail

: "${ROLE:=worker}"

: "${MPI_BIN:=/opt/nbody/n_body_simulation_mpi}"
: "${MPI_ARGS:=}"
: "${HOSTS:=}"
: "${HOSTFILE:=/opt/nbody/hosts}"

: "${SSH_PORT:=2222}"

: "${SSHD_PORT:=22}"

: "${SSH_PRIVATE_KEY:=}"
: "${SSH_PUBLIC_KEY:=}"

: "${ONLY_RANK0_OUTPUT:=1}"

: "${OMPI_TCP_IF_INCLUDE:=}"
: "${OMPI_TCP_IF_EXCLUDE:=lo,docker0,virbr0,br0}"

mkdir -p /var/run/sshd /root/.ssh
chmod 700 /root/.ssh

cat >/root/.ssh/config <<EOF
Host *
  Port ${SSH_PORT}
  StrictHostKeyChecking no
  UserKnownHostsFile=/dev/null
  LogLevel ERROR
EOF
chmod 600 /root/.ssh/config

if [[ -n "${SSH_PUBLIC_KEY}" ]]; then
  echo "${SSH_PUBLIC_KEY}" >> /root/.ssh/authorized_keys
  chmod 600 /root/.ssh/authorized_keys
fi

if [[ -n "${SSH_PRIVATE_KEY}" ]]; then
  umask 077
  printf "%s\n" "${SSH_PRIVATE_KEY}" > /root/.ssh/id_rsa
  chmod 600 /root/.ssh/id_rsa
fi

sed -i "s/^#\?Port .*/Port ${SSHD_PORT}/" /etc/ssh/sshd_config
grep -q '^ListenAddress ' /etc/ssh/sshd_config || echo "ListenAddress 0.0.0.0" >> /etc/ssh/sshd_config

ssh-keygen -A >/dev/null 2>&1 || true
/usr/sbin/sshd

if [[ "${ROLE}" == "worker" ]]; then
  echo "[start-mpi.sh] ROLE=worker: sshd running (listening on ${SSHD_PORT})."
  exec tail -f /dev/null
fi

if [[ -z "${SSH_PRIVATE_KEY}" ]]; then
  echo "ERROR: ROLE=launcher requires SSH_PRIVATE_KEY." >&2
  exit 2
fi

if [[ -z "${HOSTS}" ]]; then
  echo "ERROR: ROLE=launcher requires HOSTS (space-separated host IPs/DNS names)." >&2
  exit 3
fi

if [[ ! -x "${MPI_BIN}" ]]; then
  echo "ERROR: MPI binary not executable at: ${MPI_BIN}" >&2
  exit 4
fi

printf "%s\n" ${HOSTS} > "${HOSTFILE}"

if [[ -z "${OMPI_TCP_IF_INCLUDE}" ]]; then
  first_remote=""
  while read -r h; do
    [[ -z "${h}" ]] && continue
    if [[ "${h}" != "localhost" && "${h}" != "127.0.0.1" ]]; then
      first_remote="${h}"
      break
    fi
  done < "${HOSTFILE}"

  if [[ -n "${first_remote}" ]] && command -v ip >/dev/null 2>&1; then
    OMPI_TCP_IF_INCLUDE="$(ip route get "${first_remote}" 2>/dev/null | awk '/ dev /{for (i=1;i<=NF;i++) if ($i=="dev"){print $(i+1); exit}}')"
  fi
fi

if [[ -n "${OMPI_TCP_IF_INCLUDE}" ]]; then
  echo "[start-mpi.sh] OpenMPI will use interface: ${OMPI_TCP_IF_INCLUDE}"
else
  echo "WARNING: Could not auto-detect OMPI_TCP_IF_INCLUDE. You should set it (e.g. eth0/eno1)." >&2
fi

echo "[start-mpi.sh] Checking SSH connectivity to each *remote* host on port ${SSH_PORT}..."
while read -r h; do
  [[ -z "${h}" ]] && continue

  if [[ "${h}" == "localhost" || "${h}" == "127.0.0.1" ]]; then
    echo "  - ${h} (local, skip SSH check)"
    continue
  fi
  echo "  - ${h}"
  ssh -o BatchMode=yes -p "${SSH_PORT}" "root@${h}" "true" || {
    echo "ERROR: Cannot SSH to ${h} on port ${SSH_PORT}." >&2
    exit 5
  }
done < "${HOSTFILE}"

NP=$(grep -cve '^\s*$' "${HOSTFILE}" | tr -d ' ')
echo "[start-mpi.sh] Launching MPI: 1 rank per host, total ranks=${NP}"

MPICMD=(
  mpirun
  --allow-run-as-root
  --hostfile "${HOSTFILE}"
  -np "${NP}"
  -map-by ppr:1:node
  --tag-output

  --mca plm_rsh_agent "ssh -p ${SSH_PORT} -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"

  --mca btl tcp,self
  --mca oob tcp
  --mca btl_tcp_if_exclude "${OMPI_TCP_IF_EXCLUDE}"
  --mca oob_tcp_if_exclude "${OMPI_TCP_IF_EXCLUDE}"
)

if [[ -n "${OMPI_TCP_IF_INCLUDE}" ]]; then
  MPICMD+=( --mca btl_tcp_if_include "${OMPI_TCP_IF_INCLUDE}" )
  MPICMD+=( --mca oob_tcp_if_include "${OMPI_TCP_IF_INCLUDE}" )
fi

MPICMD+=( "${MPI_BIN}" )

if [[ -n "${MPI_ARGS}" ]]; then
  # shellcheck disable=SC2206
  MPICMD+=(${MPI_ARGS})
fi

if [[ "${ONLY_RANK0_OUTPUT}" == "1" ]]; then
  echo "[start-mpi.sh] Showing ONLY rank 0 output (set ONLY_RANK0_OUTPUT=0 to show all ranks)."
  exec "${MPICMD[@]}" 2>&1 | awk '/^\[[^,]*,0\]<(stdout|stderr)>:/ { sub(/^\[[^]]*\]<(stdout|stderr)>: /,""); print; fflush(); }'
else
  exec "${MPICMD[@]}"
fi