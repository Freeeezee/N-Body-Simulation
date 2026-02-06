#include "simulations/OpenMpSimulationAoS.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "Constants.hpp"


std::vector<Body> OpenMpSimulationAoS::calculateNextTick() {
    std::vector<Body> newBodies(bodies.size());
    const int n = static_cast<int>(bodies.size());

    #pragma omp parallel for schedule(static)
    for (int i = static_cast<int>(startIndex); i < endIndex; ++i) {
        const glm::vec3 pos = bodies[i].position;
        glm::vec3 vel = bodies[i].velocity;

        glm::vec3 totalAcceleration(0.0f);

        for (size_t j = 0; j < n; ++j) {
            const float mask = i != j ? 1.0f : 0.0f;

            const glm::vec3 direction = bodies[j].position - pos;
            const float distanceSquared = glm::dot(direction, direction) + DISTANCE_EPSILON;
            const float invDistCubed = mask / (distanceSquared * std::sqrt(distanceSquared));

            totalAcceleration += G * bodies[j].mass * invDistCubed * direction;
        }

        vel += totalAcceleration * timeStep;

        newBodies[i].position = pos + vel * timeStep;
        newBodies[i].velocity = vel;
        newBodies[i].mass = bodies[i].mass;
    }

    bodies = newBodies;
    return newBodies;
}