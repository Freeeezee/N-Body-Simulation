#!/usr/bin/env bash
set -euo pipefail

: "${ROLE:=worker}"

: "${MPI_BIN:=/opt/nbody/n_body_simulation_mpi}"
: "${MPI_ARGS:=}"
: "${HOSTS:=}"
: "${HOSTFILE:=/opt/nbody/hosts}"

: "${SSH_PORT:=2222}"
: "${SSH_PRIVATE_KEY:=}"
: "${SSH_PUBLIC_KEY:=}"

: "${ONLY_RANK0_OUTPUT:=1}"

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

ssh-keygen -A >/dev/null 2>&1 || true
/usr/sbin/sshd

if [[ "${ROLE}" == "worker" ]]; then
  echo "[start-mpi.sh] ROLE=worker: sshd running (container:22 -> host:${SSH_PORT} if published)."
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

echo "[start-mpi.sh] Checking SSH connectivity to each host on port ${SSH_PORT}..."
while read -r h; do
  [[ -z "${h}" ]] && continue
  echo "  - ${h}"
  ssh -o BatchMode=yes "root@${h}" "true" || {
    echo "ERROR: Cannot SSH to ${h} on port ${SSH_PORT}." >&2
    exit 5
  }
done < "${HOSTFILE}"

NP=$(wc -l < "${HOSTFILE}" | tr -d ' ')
echo "[start-mpi.sh] Launching MPI: 1 rank per host, total ranks=${NP}"

MPICMD=(
  mpirun
  --allow-run-as-root
  --hostfile "${HOSTFILE}"
  -np "${NP}"
  -map-by ppr:1:node
  --tag-output
  "${MPI_BIN}"
)

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