#ifndef N_BODY_SIMULATION_SIMULATION_H
#define N_BODY_SIMULATION_SIMULATION_H
#include <array>

#include "models/Body.hpp"


class Simulation {
public:
    Simulation(const float timeStep, const std::vector<Body>& bodies)
        : timeStep(timeStep), bodies(bodies) {}

    std::vector<Body> calculateNextTick();

private:
    const float timeStep;
    const std::vector<Body> bodies;
};


#endif //N_BODY_SIMULATION_SIMULATION_H