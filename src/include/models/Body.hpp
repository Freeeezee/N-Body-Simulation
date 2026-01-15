#ifndef N_BODY_SIMULATION_BODY_H
#define N_BODY_SIMULATION_BODY_H
#include "glm/vec3.hpp"

class Body {
public:
    float mass;
    glm::vec3 position;
    glm::vec3 velocity;

    Body(const float mass, const glm::vec3& position, const glm::vec3& velocity)
        : mass(mass), position(position), velocity(velocity) {}
};

#endif //N_BODY_SIMULATION_BODY_H