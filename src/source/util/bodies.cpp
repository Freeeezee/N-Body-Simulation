#include "util/bodies.h"
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

BodiesSoA1 convertBodiesToSoA1(const std::vector<Body> &bodies) {
    const size_t n = bodies.size();
    BodiesSoA1 soa;
    soa.positions.reserve(n);
    soa.velocities.reserve(n);
    soa.masses.reserve(n);

    for (const auto& body : bodies) {
        soa.positions.push_back(body.position);
        soa.velocities.push_back(body.velocity);
        soa.masses.push_back(body.mass);
    }

    return soa;
}

BodiesSoA2 convertBodiesToSoA2(const std::vector<Body> &bodies) {
    const size_t n = bodies.size();
    BodiesSoA2 soa;

    soa.posX.resize(n);
    soa.posY.resize(n);
    soa.posZ.resize(n);

    soa.velX.resize(n);
    soa.velY.resize(n);
    soa.velZ.resize(n);

    soa.masses.resize(n);

    for (size_t i = 0; i < n; ++i) {
        soa.posX[i] = bodies[i].position.x;
        soa.posY[i] = bodies[i].position.y;
        soa.posZ[i] = bodies[i].position.z;

        soa.velX[i] = bodies[i].velocity.x;
        soa.velY[i] = bodies[i].velocity.y;
        soa.velZ[i] = bodies[i].velocity.z;

        soa.masses[i] = bodies[i].mass;
    }

    return soa;
}

std::vector<Body> convertSoA1ToBodies(const BodiesSoA1 &soa) {
    const size_t n = soa.masses.size();
    std::vector<Body> bodies;
    bodies.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        bodies.emplace_back(soa.masses[i], soa.positions[i], soa.velocities[i]);
    }

    return bodies;
}

std::vector<Body> convertSoA2ToBodies(const BodiesSoA2 &soa) {
    const size_t n = soa.masses.size();
    std::vector<Body> bodies;
    bodies.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        bodies.emplace_back(
            soa.masses[i],
            glm::vec3{soa.posX[i], soa.posY[i], soa.posZ[i]},
            glm::vec3{soa.velX[i], soa.velY[i], soa.velZ[i]}
        );
    }

    return bodies;
}

