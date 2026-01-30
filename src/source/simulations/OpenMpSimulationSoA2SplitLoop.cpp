#include "simulations/OpenMpSimulationSoA2SplitLoop.hpp"
#include "Constants.hpp"
#include <cmath>
#include <vector>

inline float fastInvSqrt(float number) {
    return 1.0f / std::sqrt(number);
}

BodiesSoA2 OpenMpSimulationSoA2SplitLoop::calculateNextTick() {
    const int n = static_cast<int>(bodies.masses.size());

    std::vector<float> nextPosX(n), nextPosY(n), nextPosZ(n);
    std::vector<float> nextVelX(n), nextVelY(n), nextVelZ(n);

    std::vector<float> g_masses(n);
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        g_masses[i] = G * bodies.masses[i];
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(n); ++i) {
        const float p_ix = bodies.posX[i];
        const float p_iy = bodies.posY[i];
        const float p_iz = bodies.posZ[i];

        float accX = 0.0f;
        float accY = 0.0f;
        float accZ = 0.0f;

        auto accumulateForce = [&](int j) {
            const float dx = bodies.posX[j] - p_ix;
            const float dy = bodies.posY[j] - p_iy;
            const float dz = bodies.posZ[j] - p_iz;

            const float distSq = dx*dx + dy*dy + dz*dz + DISTANCE_EPSILON;

            const float dist = std::sqrt(distSq);
            const float invDist = 1.0f / dist;
            const float invDistCube = invDist * invDist * invDist;

            const float s = g_masses[j] * invDistCube;

            accX += s * dx;
            accY += s * dy;
            accZ += s * dz;
        };

        #pragma omp simd
        for (int j = 0; j < i; ++j) {
            accumulateForce(j);
        }

        #pragma omp simd
        for (int j = i + 1; j < static_cast<int>(n); ++j) {
            accumulateForce(j);
        }

        float v_ix = bodies.velX[i] + accX * timeStep;
        float v_iy = bodies.velY[i] + accY * timeStep;
        float v_iz = bodies.velZ[i] + accZ * timeStep;

        nextPosX[i] = p_ix + v_ix * timeStep;
        nextPosY[i] = p_iy + v_iy * timeStep;
        nextPosZ[i] = p_iz + v_iz * timeStep;

        nextVelX[i] = v_ix;
        nextVelY[i] = v_iy;
        nextVelZ[i] = v_iz;
    }

    bodies.posX = std::move(nextPosX);
    bodies.posY = std::move(nextPosY);
    bodies.posZ = std::move(nextPosZ);
    bodies.velX = std::move(nextVelX);
    bodies.velY = std::move(nextVelY);
    bodies.velZ = std::move(nextVelZ);

    return bodies;
}