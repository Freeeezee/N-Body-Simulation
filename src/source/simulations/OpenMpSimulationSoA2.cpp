#include "simulations/OpenMpSimulationSoA2.hpp"
#include "Constants.hpp"
#include <cmath>


BodiesSoA2 OpenMpSimulationSoA2::calculateNextTick() {
    const size_t n = bodies.masses.size();

    std::vector<float> nextPosX(n), nextPosY(n), nextPosZ(n);
    std::vector<float> nextVelX(n), nextVelY(n), nextVelZ(n);

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(n); ++i) {
        const float p_ix = bodies.posX[i];
        const float p_iy = bodies.posY[i];
        const float p_iz = bodies.posZ[i];

        float v_ix = bodies.velX[i];
        float v_iy = bodies.velY[i];
        float v_iz = bodies.velZ[i];

        float accX = 0.0f, accY = 0.0f, accZ = 0.0f;

        for (int j = 0; j < static_cast<int>(n); ++j) {
            const float mask = i != j ? 1.0f : 0.0f;

            const float dx = bodies.posX[j] - p_ix;
            const float dy = bodies.posY[j] - p_iy;
            const float dz = bodies.posZ[j] - p_iz;

            const float distSq = dx*dx + dy*dy + dz*dz + DISTANCE_EPSILON;
            const float invDistCube = mask / (distSq * std::sqrt(distSq));
            const float s = G * bodies.masses[j] * invDistCube;

            accX += s * dx;
            accY += s * dy;
            accZ += s * dz;
        }

        v_ix += accX * timeStep;
        v_iy += accY * timeStep;
        v_iz += accZ * timeStep;

        nextPosX[i] = p_ix + v_ix * timeStep;
        nextPosY[i] = p_iy + v_iy * timeStep;
        nextPosZ[i] = p_iz + v_iz * timeStep;

        nextVelX[i] = v_ix;
        nextVelY[i] = v_iy;
        nextVelZ[i] = v_iz;
    }

    bodies.posX.swap(nextPosX);
    bodies.posY.swap(nextPosY);
    bodies.posZ.swap(nextPosZ);

    bodies.velX.swap(nextVelX);
    bodies.velY.swap(nextVelY);
    bodies.velZ.swap(nextVelZ);

    return bodies;
}