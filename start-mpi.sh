#!/usr/bin/env bash
set -euo pipefail

APP="/opt/nbody/n_body_simulation_mpi"
SERVICE_DNS_NAME="${MPI_SERVICE_DNS:-tasks.mpi}"
HOSTFILE="${MPI_HOSTFILE:-/tmp/hostfile}"
NP="${MPI_NP:-0}"
EXTRA_MPIRUN_ARGS="${MPI_EXTRA_ARGS:-}"
SLOTS_PER_HOST="${MPI_SLOTS_PER_HOST:-1}"

OVERLAY_CIDR_PREFIX="${MPI_OVERLAY_PREFIX:-10.}"

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

sshd_running() {
  pgrep -x sshd >/dev/null 2>&1 || ss -ltn 2>/dev/null | grep -qE '(:22\s)'
}

start_sshd_if_needed() {
  if sshd_running; then
    return 0
  fi
  /usr/sbin/sshd -D -e &
}

discover_hosts() {
  getent ahostsv4 "${SERVICE_DNS_NAME}" \
    | awk '{print $1}' \
    | sort -u \
    | awk -v s="${SLOTS_PER_HOST}" '{print $1 " slots=" s}' > "${HOSTFILE}"

  echo "Discovered hosts (${SERVICE_DNS_NAME}) -> ${HOSTFILE}:"
  nl -ba "${HOSTFILE}"
}

detect_overlay_iface() {
  ip -o -4 addr show \
    | awk -v pfx="${OVERLAY_CIDR_PREFIX}" '
        $0 ~ (" " pfx) {
          print $2; exit
        }'
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
    np="$(( host_count * SLOTS_PER_HOST ))"
  fi

  local overlay_if
  overlay_if="$(detect_overlay_iface || true)"
  if [[ -z "${overlay_if}" ]]; then
    echo "ERROR: Could not detect overlay interface (prefix ${OVERLAY_CIDR_PREFIX})." >&2
    echo "Tip: set MPI_OVERLAY_PREFIX to match your overlay subnet (e.g. 10.0. or 192.168.)." >&2
    ip -o -4 addr show >&2
    exit 4
  fi

  echo "Launching OpenMPI: np=${np}, hosts=${host_count}, slots_per_host=${SLOTS_PER_HOST}, iface=${overlay_if}"

  exec mpirun \
    --allow-run-as-root \
    --hostfile "${HOSTFILE}" \
    -np "${np}" \
    --map-by "ppr:${SLOTS_PER_HOST}:node" \
    --bind-to none \
    --mca plm_rsh_agent "ssh" \
    --mca oob_tcp_if_include "${overlay_if}" \
    --mca btl tcp,self \
    --mca btl_tcp_if_include "${overlay_if}" \
    --mca oob_tcp_disable_family ipv6 \
    --report-bindings \
    ${EXTRA_MPIRUN_ARGS} \
    "${APP}"
}

main() {
  setup_ssh
  start_sshd_if_needed

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