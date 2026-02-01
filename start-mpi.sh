#!/usr/bin/env bash
set -euo pipefail

APP="/opt/nbody/n_body_simulation_mpi"
SERVICE_DNS_NAME="${MPI_SERVICE_DNS:-tasks.mpi}"
HOSTFILE="${MPI_HOSTFILE:-/tmp/hostfile}"
NP="${MPI_NP:-0}"
EXTRA_MPIRUN_ARGS="${MPI_EXTRA_ARGS:-}"

setup_ssh() {
  mkdir -p /var/run/sshd /root/.ssh
  chmod 700 /root/.ssh

  if [[ -f /opt/nbody/mpi_key ]]; then
    install -m 600 /opt/nbody/mpi_key /root/.ssh/id_rsa
  elif [[ -f /mpi_key ]]; then
    install -m 600 /mpi_key /root/.ssh/id_rsa
  fi

  if [[ -f /opt/nbody/mpi_key.pub ]]; then
    install -m 600 /opt/nbody/mpi_key.pub /root/.ssh/authorized_keys
  elif [[ -f /mpi_key.pub ]]; then
    install -m 600 /mpi_key.pub /root/.ssh/authorized_keys
  fi

  cat > /root/.ssh/config <<'EOF'
Host *
  StrictHostKeyChecking no
  UserKnownHostsFile /dev/null
  LogLevel ERROR
  ServerAliveInterval 30
  ServerAliveCountMax 6
EOF
  chmod 600 /root/.ssh/config
}

start_sshd() {
  /usr/sbin/sshd -D -e &
}

discover_hosts() {
  getent ahostsv4 "${SERVICE_DNS_NAME}" \
    | awk '{print $1}' \
    | sort -u > "${HOSTFILE}"

  echo "Discovered hosts (${SERVICE_DNS_NAME}) -> ${HOSTFILE}:"
  nl -ba "${HOSTFILE}"
}

run_mpi() {
  if [[ ! -x "${APP}" ]]; then
    echo "ERROR: MPI binary not found/executable: ${APP}" >&2
    exit 1
  fi

  discover_hosts

  local host_count
  host_count="$(wc -l < "${HOSTFILE}" | tr -d ' ')"
  if [[ "${host_count}" -le 0 ]]; then
    echo "ERROR: No hosts discovered from ${SERVICE_DNS_NAME}" >&2
    exit 2
  fi

  local np="${NP}"
  if [[ "${np}" -le 0 ]]; then
    np="${host_count}"
  fi

  echo "Launching OpenMPI: np=${np}, hosts=${host_count}"
  exec mpirun \
    --allow-run-as-root \
    --hostfile "${HOSTFILE}" \
    -np "${np}" \
    --mca plm_rsh_agent "ssh" \
    ${EXTRA_MPIRUN_ARGS} \
    "${APP}"
}

main() {
  setup_ssh
  start_sshd

  case "${1:-daemon}" in
    run)
      shift || true
      run_mpi "$@"
      ;;
    daemon|sshd)
      wait -n
      ;;
    *)
      exec "$@"
      ;;
  esac
}

main "$@"