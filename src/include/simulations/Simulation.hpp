#ifndef N_BODY_SIMULATION_SIMULATION_H
#define N_BODY_SIMULATION_SIMULATION_H

#include "models/Body.hpp"


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


#endif //N_BODY_SIMULATION_SIMULATION_H