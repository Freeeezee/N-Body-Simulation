#include "simulations/OpenMpSingleLoopSimulationSoA2.hpp"
#include "Constants.hpp"
#include <cmath>
#include <vector>

#include "models/Body.hpp"

BodiesSoA2 OpenMpSingleLoopSimulationSoA2::calculateNextTick() {
    const int n = static_cast<int>(bodies.masses.size());
    const long long acc_count = static_cast<long long>(n) * (n - 1) / 2;

    std::vector<float> accX(n, 0.0f);
    std::vector<float> accY(n, 0.0f);
    std::vector<float> accZ(n, 0.0f);

    #pragma omp parallel for schedule(guided)
    for (long long k = 0; k < acc_count; ++k) {
        const long i = static_cast<long>((1.0 + std::sqrt(1.0 + 8.0 * k)) / 2.0);
        const long j = k - i * (i - 1) / 2;

        const float dx = bodies.posX[j] - bodies.posX[i];
        const float dy = bodies.posY[j] - bodies.posY[i];
        const float dz = bodies.posZ[j] - bodies.posZ[i];

        const float distSq = dx*dx + dy*dy + dz*dz + DISTANCE_EPSILON;

        const float invDistCubed = 1.0f / (distSq * std::sqrt(distSq));

        const float f_base = invDistCubed;

        const float s_i = G * bodies.masses[j] * f_base;
        const float s_j = G * bodies.masses[i] * f_base;

        #pragma omp atomic
        accX[i] += s_i * dx;
        #pragma omp atomic
        accY[i] += s_i * dy;
        #pragma omp atomic
        accZ[i] += s_i * dz;

        #pragma omp atomic
        accX[j] -= s_j * dx;
        #pragma omp atomic
        accY[j] -= s_j * dy;
        #pragma omp atomic
        accZ[j] -= s_j * dz;
    }

    std::vector<float> nextPosX(n), nextPosY(n), nextPosZ(n);
    std::vector<float> nextVelX(n), nextVelY(n), nextVelZ(n);

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        float v_ix = bodies.velX[i] + accX[i] * timeStep;
        float v_iy = bodies.velY[i] + accY[i] * timeStep;
        float v_iz = bodies.velZ[i] + accZ[i] * timeStep;

        nextPosX[i] = bodies.posX[i] + v_ix * timeStep;
        nextPosY[i] = bodies.posY[i] + v_iy * timeStep;
        nextPosZ[i] = bodies.posZ[i] + v_iz * timeStep;

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
