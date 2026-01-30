#include "simulations/OpenMpSingleLoopSimulation.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "Constants.hpp"
#include <cmath>

std::vector<Body> OpenMpSingleLoopSimulation::calculateNextTick() {
    std::vector<Body> newBodies = bodies;
    const int n = static_cast<int>(bodies.size());
    const long acc_count = (n - 1) * n / 2;

    std::vector totalAccelerations(n, glm::vec3(0.0f));

    #pragma omp parallel for schedule(dynamic, 64)
    for (long k = 0; k < acc_count; ++k) {
        long i = static_cast<long>((1.0 + std::sqrt(1.0 + 8.0 * k)) / 2.0);
        long j = k - i * (i - 1) / 2;

        const glm::vec3 posI = bodies[i].position;
        const glm::vec3 posJ = bodies[j].position;

        const glm::vec3 r_ij = posJ - posI; // Vector from i to j
        const float distSqr = glm::dot(r_ij, r_ij) + DISTANCE_EPSILON;

        const float invDistCubed = 1.0f / (distSqr * std::sqrt(distSqr));

        const glm::vec3 f_base = r_ij * invDistCubed;

        const glm::vec3 acc_i = f_base * (G * bodies[j].mass);

        const glm::vec3 acc_j = f_base * (-G * bodies[i].mass);

        #pragma omp atomic
        totalAccelerations[i].x += acc_i.x;
        #pragma omp atomic
        totalAccelerations[i].y += acc_i.y;
        #pragma omp atomic
        totalAccelerations[i].z += acc_i.z;

        #pragma omp atomic
        totalAccelerations[j].x += acc_j.x;
        #pragma omp atomic
        totalAccelerations[j].y += acc_j.y;
        #pragma omp atomic
        totalAccelerations[j].z += acc_j.z;
    }

    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        newBodies[i].velocity += totalAccelerations[i] * timeStep;
        newBodies[i].position += newBodies[i].velocity * timeStep;
    }

    bodies = newBodies;
    return newBodies;
}