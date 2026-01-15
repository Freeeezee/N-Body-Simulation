#include "Simulation.hpp"

#include <iostream>

#include "glm/ext/quaternion_geometric.hpp"

float G = 6.67430e-11f; // Gravitational constant

std::vector<Body> Simulation::calculateNextTick() {
	std::vector<Body> newBodies;
	for (const auto& body : bodies) {
		Body newBody = body;
		// Update position based on velocity
		newBody.position.x += newBody.velocity.x * timeStep;
		newBody.position.y += newBody.velocity.y * timeStep;
		newBody.position.z += newBody.velocity.z * timeStep;

		// Calculating acceleration due to gravity from other bodies
		glm::vec3 totalAcceleration(0.0f);
		for (const auto& otherBody : bodies) {
			if (&body != &otherBody) {
				glm::vec3 direction = otherBody.position - body.position;
				const float distanceSquared = glm::dot(direction, direction) + 1e-12f; // Avoid division by zero
				const float forceMagnitude = (G * otherBody.mass) / distanceSquared;
				const glm::vec3 acceleration = forceMagnitude * glm::normalize(direction);
				totalAcceleration += acceleration;
			}
		}
		
		// Update velocity based on acceleration
		newBody.velocity += totalAcceleration * timeStep;
		
		newBodies.push_back(newBody);
	}

	bodies = newBodies;
	return newBodies;
}