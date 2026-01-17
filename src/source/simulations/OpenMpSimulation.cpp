#include "simulations/OpenMpSimulation.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "Constants.hpp"


BodiesSoA OpenMpSimulation::calculateNextTick() {
    const size_t n = bodies.masses.size();
    BodiesSoA nextState;
    nextState.positions.resize(n);
    nextState.velocities.resize(n);
    nextState.masses = bodies.masses;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        const glm::vec3 pos = bodies.positions[i];
        glm::vec3 vel = bodies.velocities[i];

        glm::vec3 totalAcceleration(0.0f);

        for (size_t j = 0; j < n; ++j) {
            if (i == j) continue;

            const glm::vec3 direction = bodies.positions[j] - pos;
            const float distanceSquared = glm::dot(direction, direction) + DISTANCE_EPSILON;
            const float invDistCubed = 1.0f / (distanceSquared * std::sqrt(distanceSquared));

            totalAcceleration += G * bodies.masses[j] * invDistCubed * direction;
        }

        vel += totalAcceleration * timeStep;

        nextState.positions[i] = pos + vel * timeStep;
        nextState.velocities[i] = vel;
    }

    bodies = nextState;
    return nextState;
}