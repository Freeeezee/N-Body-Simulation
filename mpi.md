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

## Manager – Deploy

```shell
docker stack deploy -c swarm-mpi.yml nbody
docker service ls
docker service ps nbody_mpi
```

## Manager – Run

```shell
./run_swarm_mpi.sh nbody mpi
```
