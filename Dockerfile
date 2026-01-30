FROM ubuntu:24.04 AS build
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates git \
    build-essential cmake ninja-build pkg-config \
    openmpi-bin libopenmpi-dev \
    ocl-icd-opencl-dev \
    libgl1-mesa-dev libglu1-mesa-dev \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B /build -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DGLFW_BUILD_WAYLAND=OFF \
        -DGLFW_BUILD_X11=ON \
 && cmake --build /build --target n_body_simulation_mpi -j

FROM ubuntu:24.04 AS runtime
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    openmpi-bin libopenmpi3 \
    ocl-icd-libopencl1 \
    libgl1 libglu1-mesa \
    libx11-6 libxrandr2 libxinerama1 libxcursor1 libxi6 \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /var/run/sshd /root/.ssh \
 && chmod 700 /root/.ssh

RUN sed -i 's/^#\?PermitRootLogin .*/PermitRootLogin yes/' /etc/ssh/sshd_config \
 && sed -i 's/^#\?PasswordAuthentication .*/PasswordAuthentication no/' /etc/ssh/sshd_config \
 && sed -i 's/^#\?KbdInteractiveAuthentication .*/KbdInteractiveAuthentication no/' /etc/ssh/sshd_config \
 && sed -i 's/^#\?UsePAM .*/UsePAM no/' /etc/ssh/sshd_config

WORKDIR /opt/nbody

COPY --from=build /build/n_body_simulation_mpi ./n_body_simulation_mpi
COPY --from=build /src/src/shader ./shader
COPY --from=build /src/src/kernels ./kernels

COPY start-mpi.sh /opt/nbody/start-mpi.sh
RUN sed -i 's/\r$//' /opt/nbody/start-mpi.sh \
 && chmod +x /opt/nbody/start-mpi.sh

EXPOSE 22
ENTRYPOINT ["/opt/nbody/start-mpi.sh"]