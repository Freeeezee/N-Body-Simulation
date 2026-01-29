#include "simulations/OpenMpSimulationSoA2SplitLoop.hpp"
#include "Constants.hpp"
#include <cmath>
#include <vector>

// Helper for Fast Inverse Square Root (Quake III style or AVX intrinsic preferred usually)
// For standard C++, this inline function helps the compiler optimize.
inline float fastInvSqrt(float number) {
    // Exact standard square root is safer, but 1/sqrt is the target for optimization.
    // If you enable -Ofast or -ffast-math in your compiler,
    // std::sqrt usually becomes a hardware rsqrt instruction automatically.
    return 1.0f / std::sqrt(number);
}

BodiesSoA2 OpenMpSimulationSoA2SplitLoop::calculateNextTick() {
    const size_t n = bodies.masses.size();

    std::vector<float> nextPosX(n), nextPosY(n), nextPosZ(n);
    std::vector<float> nextVelX(n), nextVelY(n), nextVelZ(n);

    // Optimization 3: Pre-calculate G * mass to save N^2 multiplications
    std::vector<float> g_masses(n);
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; ++i) {
        g_masses[i] = G * bodies.masses[i];
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(n); ++i) {
        const float p_ix = bodies.posX[i];
        const float p_iy = bodies.posY[i];
        const float p_iz = bodies.posZ[i];

        // Accumulators for force/acceleration
        float accX = 0.0f;
        float accY = 0.0f;
        float accZ = 0.0f;

        // Optimization 1: Loop Splitting
        // We define a lambda or macro to avoid code duplication for the two loops
        auto accumulateForce = [&](int j) {
            const float dx = bodies.posX[j] - p_ix;
            const float dy = bodies.posY[j] - p_iy;
            const float dz = bodies.posZ[j] - p_iz;

            const float distSq = dx*dx + dy*dy + dz*dz + DISTANCE_EPSILON;

            // Optimization 2: Reduced Division cost
            // Instead of 1.0 / (distSq * sqrt(distSq)), we use pow or fast math if flags allow.
            // Mathematically: invDistCube = (distSq)^(-3/2)
            // A common manual optimization without intrinsics:
            const float dist = std::sqrt(distSq);
            const float invDist = 1.0f / dist;
            const float invDistCube = invDist * invDist * invDist;

            // Note: If you compile with -Ofast, the compiler will likely optimize
            // the standard line better than manual tricks, but splitting the loop
            // helps the vectorizer significantly.

            const float s = g_masses[j] * invDistCube;

            accX += s * dx;
            accY += s * dy;
            accZ += s * dz;
        };

        // Loop 1: 0 to i-1
        // #pragma omp simd ensures the compiler uses AVX/SSE instructions here
        #pragma omp simd
        for (int j = 0; j < i; ++j) {
            accumulateForce(j);
        }

        // Loop 2: i+1 to n
        #pragma omp simd
        for (int j = i + 1; j < static_cast<int>(n); ++j) {
            accumulateForce(j);
        }

        // Integration (Symplectic Euler)
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

    // Update state
    bodies.posX = std::move(nextPosX);
    bodies.posY = std::move(nextPosY);
    bodies.posZ = std::move(nextPosZ);
    bodies.velX = std::move(nextVelX);
    bodies.velY = std::move(nextVelY);
    bodies.velZ = std::move(nextVelZ);

    return bodies;
}