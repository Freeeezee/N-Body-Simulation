#include "simulations/OpenMpSingleLoopSimulationSoA2.hpp"
#include "Constants.hpp"
#include <cmath>
#include <vector>
#include <algorithm> // for std::max if needed

BodiesSoA2 OpenMpSingleLoopSimulationSoA2::calculateNextTick() {
    const size_t n = bodies.masses.size();
    const long long acc_count = static_cast<long long>(n) * (n - 1) / 2;

    // Temporary accumulators for acceleration (or force)
    // We cannot write directly to velocities yet because velocities are needed
    // for the integration step after force calculation, and atomics on the
    // live velocity vector might dirty the cache for the integration step.
    std::vector<float> accX(n, 0.0f);
    std::vector<float> accY(n, 0.0f);
    std::vector<float> accZ(n, 0.0f);

    // 1. Single Loop Force Calculation
    #pragma omp parallel for schedule(guided)
    for (long long k = 0; k < acc_count; ++k) {
        // Inverse triangular mapping: Map linear k -> (i, j) where i > j
        // Formula: i = floor((1 + sqrt(1 + 8k)) / 2)
        const long i = static_cast<long>((1.0 + std::sqrt(1.0 + 8.0 * k)) / 2.0);
        const long j = k - i * (i - 1) / 2;

        // Load Positions (SoA layout improves cache hits here)
        const float dx = bodies.posX[j] - bodies.posX[i];
        const float dy = bodies.posY[j] - bodies.posY[i];
        const float dz = bodies.posZ[j] - bodies.posZ[i];

        const float distSq = dx*dx + dy*dy + dz*dz + DISTANCE_EPSILON;

        // rsqrt is faster than 1.0/sqrt if you have fast-math enabled,
        // but we stick to standard C++ here.
        const float invDistCubed = 1.0f / (distSq * std::sqrt(distSq));

        // Base scalar for force (excluding masses for now)
        const float f_base = invDistCubed;

        // Calculate acceleration contributions
        // Acceleration on I = (G * m_j / r^3) * vec_ij
        // Acceleration on J = (G * m_i / r^3) * -vec_ij

        const float s_i = G * bodies.masses[j] * f_base;
        const float s_j = G * bodies.masses[i] * f_base;

        // Apply to Body I (Atomic required due to race conditions on index i)
        #pragma omp atomic
        accX[i] += s_i * dx;
        #pragma omp atomic
        accY[i] += s_i * dy;
        #pragma omp atomic
        accZ[i] += s_i * dz;

        // Apply to Body J (Newton's 3rd Law: Opposite direction)
        // dx is (posJ - posI), so Force on J is opposite.
        // We subtract the vector components.
        #pragma omp atomic
        accX[j] -= s_j * dx;
        #pragma omp atomic
        accY[j] -= s_j * dy;
        #pragma omp atomic
        accZ[j] -= s_j * dz;
    }

    // Prepare next state buffers
    std::vector<float> nextPosX(n), nextPosY(n), nextPosZ(n);
    std::vector<float> nextVelX(n), nextVelY(n), nextVelZ(n);

    // 2. Integration Step (Fully Parallel, No Data Races)
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; ++i) {
        // Symplectic Euler (or similar semi-implicit scheme)
        // v_new = v_old + a * dt
        float v_ix = bodies.velX[i] + accX[i] * timeStep;
        float v_iy = bodies.velY[i] + accY[i] * timeStep;
        float v_iz = bodies.velZ[i] + accZ[i] * timeStep;

        // pos_new = pos_old + v_new * dt
        nextPosX[i] = bodies.posX[i] + v_ix * timeStep;
        nextPosY[i] = bodies.posY[i] + v_iy * timeStep;
        nextPosZ[i] = bodies.posZ[i] + v_iz * timeStep;

        nextVelX[i] = v_ix;
        nextVelY[i] = v_iy;
        nextVelZ[i] = v_iz;
    }

    // 3. Swap buffers
    bodies.posX.swap(nextPosX);
    bodies.posY.swap(nextPosY);
    bodies.posZ.swap(nextPosZ);

    bodies.velX.swap(nextVelX);
    bodies.velY.swap(nextVelY);
    bodies.velZ.swap(nextVelZ);

    return bodies;
}