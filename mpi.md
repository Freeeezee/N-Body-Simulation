# MPI Docker

### Build Container:
``docker build -t nbody-mpi .``

### Run Worker:
Mac/Linux:
```shell
docker run -d --name nbody-worker --restart unless-stopped \
  -p 2222:22 \
  -e ROLE=worker \
  -e SSH_PUBLIC_KEY="$(cat mpi_key.pub)" \
  nbody-mpi
```

Linux (network host):
```shell
docker run -d --name nbody-worker --restart unless-stopped \
  --network host \
  -e ROLE=worker \
  -e SSHD_PORT=2222 \
  -e SSH_PUBLIC_KEY="$(cat mpi_key.pub)" \
  nbody-mpi
```

Windows (Powershell):
```powershell
$pub = Get-Content -Raw .\mpi_key.pub
docker run -d --name nbody-worker --restart unless-stopped `
  -p 2222:22 `
  -e ROLE=worker `
  -e SSH_PUBLIC_KEY="$pub" `
  nbody-mpi
```

### Run Master:
```shell
docker run --rm --name nbody-launcher \
  -p 2222:22 \
  -e ROLE=launcher \
  -e SSH_PORT=2222 \
  -e HOSTS="IP1 IP2 IP3" \
  -e SSH_PRIVATE_KEY="$(cat mpi_key)" \
  -e SSH_PUBLIC_KEY="$(cat mpi_key.pub)" \
  -e ONLY_RANK0_OUTPUT=1 \
  -e MPI_ARGS="" \
  nbody-mpi
```

Linux (network host):
```shell
docker run --rm --name nbody-launcher \
  --network host \
  -e ROLE=launcher \
  -e SSH_PORT=2222 \
  -e SSHD_PORT=2222 \
  -e HOSTS="IP1 IP2 IP3" \
  -e SSH_PRIVATE_KEY="$(cat mpi_key)" \
  -e SSH_PUBLIC_KEY="$(cat mpi_key.pub)" \
  -e ONLY_RANK0_OUTPUT=0 \
  nbody-mpi
```