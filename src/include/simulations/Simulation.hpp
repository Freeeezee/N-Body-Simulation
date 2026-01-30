#ifndef N_BODY_SIMULATION_SIMULATION_H
#define N_BODY_SIMULATION_SIMULATION_H

#include "models/Body.hpp"
#include "util/bodies.h"
#include <vector>


class Simulation {
public:
    virtual ~Simulation() = default;

    Simulation(
        const float timeStep,
        const std::vector<Body>& bodies,
        const unsigned long startIndex = 0,
        const unsigned long endIndex = -1
        ) :
    timeStep(timeStep),
    bodies(bodies),
    startIndex(startIndex),
    endIndex(endIndex == -1 ? bodies.size() : endIndex)
    {}

    virtual std::vector<Body> calculateNextTick() = 0;

    void setRange(const unsigned long start, const unsigned long end) {
        this->startIndex = start;
        this->endIndex = end;
    }

    void setBodies(const std::vector<Body>& newBodies) { this->bodies = newBodies; }

protected:
    const float timeStep;
    std::vector<Body> bodies;

    unsigned long startIndex;
    unsigned long endIndex;
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