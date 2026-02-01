# MPI

## Manager – Initialize

```shell
MANAGER_IP="<manager-ip>"
docker swarm init --advertise-addr "${MANAGER_IP}"
```

## Worker – Join

```shell
docker swarm join-token worker
```

## Build

```shell
docker build -t nbody-mpi:latest .
```

## Manager – Run

```shell
./run_swarm_mpi.sh nbody mpi
```
