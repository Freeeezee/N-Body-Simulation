#!/usr/bin/env bash
set -euo pipefail

STACK="${1:-nbody}"
SERVICE="${2:-mpi}"
FULL_SERVICE="${STACK}_${SERVICE}"

CID="$(docker ps --filter "name=${FULL_SERVICE}" --format '{{.ID}}' | head -n 1)"
if [[ -z "${CID}" ]]; then
  echo "No running containers found for service ${FULL_SERVICE}" >&2
  exit 1
fi

docker exec -e MPI_SERVICE_DNS="tasks.${SERVICE}" "${CID}" /opt/nbody/start-mpi.sh run