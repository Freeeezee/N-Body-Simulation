#include "simulations/OpenMpSimulation.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "Constants.hpp"


std::vector<Body> OpenMpSimulation::calculateNextTick() {
    std::vector<Body> newBodies(bodies.size());

    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < bodies.size(); ++i) {
        const auto& body = bodies[i];
        auto newBody = Body(body);

        newBody.position.x += newBody.velocity.x * timeStep;
        newBody.position.y += newBody.velocity.y * timeStep;
        newBody.position.z += newBody.velocity.z * timeStep;

        glm::vec3 totalAcceleration(0.0f);

        for (size_t j = 0; j < bodies.size(); ++j) {
            if (i != j) {
                const auto& otherBody = bodies[j];
                glm::vec3 direction = otherBody.position - body.position;
                const float distanceSquared = glm::dot(direction, direction) + DISTANCE_EPSILON;
                const float forceMagnitude = (G * otherBody.mass) / distanceSquared;
                const glm::vec3 acceleration = forceMagnitude * glm::normalize(direction);
                totalAcceleration += acceleration;
            }
        }

        newBody.velocity += totalAcceleration * timeStep;

        newBodies[i] = newBody;
    }

    bodies = newBodies;
    return newBodies;
}