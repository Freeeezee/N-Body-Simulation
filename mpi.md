# MPI

## Manager – Initialize

```shell
MANAGER_IP="<manager-ip>"
sudo docker swarm init --advertise-addr "${MANAGER_IP}"
```

## Worker – Join

Run command received from manager initialization.

## Build

```shell
sudo docker build -t nbody-mpi:latest .
```

## Manager – Deploy

```shell
sudo docker stack rm nbody
sudo docker stack deploy -c swarm-mpi.yml nbody
sudo docker service ls
sudo docker service ps nbody_mpi
```

## Manager – Run

```shell
sudo ./run_swarm_mpi.sh nbody mpi
```
