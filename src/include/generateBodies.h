#ifndef N_BODY_SIMULATION_GENERATEBODIES_H
#define N_BODY_SIMULATION_GENERATEBODIES_H
#include <vector>

#include "models/Body.hpp"

std::vector<Body> generateBodies(
    int number,
    glm::vec3 positionsLower,
    glm::vec3 positionsUpper,
    glm::vec3 velocitiesLower,
    glm::vec3 velocitiesUpper
);

#endif //N_BODY_SIMULATION_GENERATEBODIES_H
