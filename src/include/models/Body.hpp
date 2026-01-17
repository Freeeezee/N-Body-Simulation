#ifndef N_BODY_SIMULATION_BODY_H
#define N_BODY_SIMULATION_BODY_H
#include "glm/vec3.hpp"

class Body {
public:
    float mass;
    glm::vec3 position;
    glm::vec3 velocity;

    Body() : mass(0.0f), position(0.0f), velocity(0.0f) {}

    explicit Body(const Body * other)
        : mass(other->mass), position(other->position), velocity(other->velocity) {}

    Body(const float mass, const glm::vec3& position, const glm::vec3& velocity)
        : mass(mass), position(position), velocity(velocity) {}
};

struct BodiesSoA {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<float> masses;
};

#endif //N_BODY_SIMULATION_BODY_H