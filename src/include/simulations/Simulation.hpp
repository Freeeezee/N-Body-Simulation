#ifndef N_BODY_SIMULATION_SIMULATION_H
#define N_BODY_SIMULATION_SIMULATION_H

#include "models/Body.hpp"
#include "util/bodies.h"


class Simulation {
public:
    virtual ~Simulation() = default;

    Simulation(const float timeStep, const std::vector<Body>& bodies)
        : timeStep(timeStep), bodies(bodies) {}

    virtual std::vector<Body> calculateNextTick() = 0;

protected:
    const float timeStep;
    std::vector<Body> bodies;
};

class SimulationSoA1 {
public:
    virtual ~SimulationSoA1() = default;

    SimulationSoA1(const float timeStep, const std::vector<Body>& bodies)
        : timeStep(timeStep), bodies(convertBodiesToSoA1(bodies)) {}

    virtual BodiesSoA1 calculateNextTick() = 0;

protected:
    const float timeStep;
    BodiesSoA1 bodies;
};

class SimulationSoA2 {
public:
    virtual ~SimulationSoA2() = default;

    SimulationSoA2(const float timeStep, const std::vector<Body>& bodies)
        : timeStep(timeStep), bodies(convertBodiesToSoA2(bodies)) {}

    virtual BodiesSoA2 calculateNextTick() = 0;

protected:
    const float timeStep;
    BodiesSoA2 bodies;
};

#endif //N_BODY_SIMULATION_SIMULATION_H