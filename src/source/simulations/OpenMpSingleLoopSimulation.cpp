#include "simulations/OpenMpSingleLoopSimulation.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "Constants.hpp"

std::vector<Body> OpenMpSingleLoopSimulation::calculateNextTick() {
    std::vector<Body> newBodies(bodies.size());
    const size_t n = bodies.size();

    const int acc_count = n * (n - 1)/2;

    std::vector<glm::vec3> partialAccellerations(acc_count, glm::vec3(0.0f));

    std::vector<glm::vec3> totalAccellerations(n, glm::vec3(0.0f));

    #pragma omp parallel for schedule(dynamic)
    for (int k = 0; k < acc_count; ++k) {
        int i = static_cast<int>(std::ceil((std::sqrt(8.0 * k + 1.0) + 1.0) / 2.0));
        int j = k - i * (i - 1) / 2;
        const glm::vec3 pos = bodies[i].position;

        const glm::vec3 direction = bodies[j].position - pos;
        const float distanceSquared = glm::dot(direction, direction) + DISTANCE_EPSILON;
        const float invDistCubed = 1.0f / (distanceSquared * std::sqrt(distanceSquared));

        partialAccellerations[k] += G * bodies[j].mass * invDistCubed * direction;
    }

    // Unteres Dreieck
    #pragma omp parallel for schedule(dynamic)
    for (int j = 0; j < n; ++j) {
        for (int i = 1; i < n; ++i) {
            int row = i * (i - 1) / 2 + j;
            totalAccellerations[j] += partialAccellerations[row];
        }
    }

    // Oberes Dreieck
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        for (int j = 1; j < n; ++j) {
            int row = i * (i - 1) / 2 + j;
            totalAccellerations[i] += partialAccellerations[row];
        }
    }

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        newBodies[i].position = bodies[i].position;
        newBodies[i].position += bodies[i].velocity * timeStep;
        newBodies[i].velocity = bodies[i].velocity + totalAccellerations[i] * timeStep;
        newBodies[i].mass = bodies[i].mass;
    }

    bodies = newBodies;
    return newBodies;
}