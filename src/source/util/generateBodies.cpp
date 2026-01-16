#include "util/generateBodies.h"
#include <random>

std::vector<Body> generateBodies(
    int number,
    float massLower,
    float massUpper,
    glm::vec3 positionsLower,
    glm::vec3 positionsUpper,
    glm::vec3 velocitiesLower,
    glm::vec3 velocitiesUpper
) {
    std::vector<Body> bodies;
    bodies.reserve(number);

    std::mt19937 rng(std::random_device{}());

    std::uniform_real_distribution px(positionsLower.x, positionsUpper.x);
    std::uniform_real_distribution py(positionsLower.y, positionsUpper.y);
    std::uniform_real_distribution pz(positionsLower.z, positionsUpper.z);

    std::uniform_real_distribution vx(velocitiesLower.x, velocitiesUpper.x);
    std::uniform_real_distribution vy(velocitiesLower.y, velocitiesUpper.y);
    std::uniform_real_distribution vz(velocitiesLower.z, velocitiesUpper.z);

    std::uniform_real_distribution massDist(massLower, massUpper);

    for (int i = 0; i < number; ++i) {
        glm::vec3 pos{px(rng), py(rng), pz(rng)};
        glm::vec3 vel{vx(rng), vy(rng), vz(rng)};
        bodies.emplace_back(massDist(rng), pos, vel);
    }

    return bodies;
}

std::vector<Body> generateBodies(
    const int number,
    const float massLower,
    const float massUpper,
    const glm::vec3 positionsLower,
    const glm::vec3 positionsUpper,
    const glm::vec3 velocitiesLower,
    const glm::vec3 velocitiesUpper,
    std::vector<Body> alwaysInclude
) {
    std::vector<Body> bodies;
    auto generatedBodies = generateBodies(
        number,
        massLower,
        massUpper,
        positionsLower,
        positionsUpper,
        velocitiesLower,
        velocitiesUpper
    );

    bodies.insert(bodies.end(), alwaysInclude.begin(), alwaysInclude.end());
    bodies.insert(bodies.end(), generatedBodies.begin(), generatedBodies.end());

    return bodies;
}
