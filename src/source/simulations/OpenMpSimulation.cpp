#include "simulations/OpenMpSimulation.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "Constants.hpp"


std::vector<Body> OpenMpSimulation::calculateNextTick() {
    std::vector<Body> newBodies(bodies.size());

    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < bodies.size(); ++i) {
        const glm::vec3 pos = bodies[i].position;
        glm::vec3 vel = bodies[i].velocity;

        glm::vec3 totalAcceleration(0.0f);

        for (size_t j = 0; j < bodies.size(); ++j) {
            if (i != j) {
                const glm::vec3 otherPos = bodies[j].position;
                const float otherMass = bodies[j].mass;

                const glm::vec3 direction = otherPos - pos;
                const float distanceSquared = glm::dot(direction, direction) + DISTANCE_EPSILON;

                const float invDistCubed = 1.0f / (distanceSquared * std::sqrt(distanceSquared));
                const glm::vec3 acceleration = G * otherMass * invDistCubed * direction;

                totalAcceleration += acceleration;
            }
        }

        vel += totalAcceleration * timeStep;
        const glm::vec3 newPos = pos + vel * timeStep;

        newBodies[i].position = newPos;
        newBodies[i].velocity = vel;
        newBodies[i].mass = bodies[i].mass;
    }

    bodies = newBodies;
    return newBodies;
}