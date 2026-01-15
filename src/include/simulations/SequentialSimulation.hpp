#ifndef N_BODY_SIMULATION_SEQUENTIALSIMULATION_H
#define N_BODY_SIMULATION_SEQUENTIALSIMULATION_H
#include "Simulation.hpp"


class SequentialSimulation : public Simulation {
public:
    SequentialSimulation(const float timeStep, const std::vector<Body>& bodies)
        : Simulation(timeStep, bodies) {}

    std::vector<Body> calculateNextTick() override;
};


#endif //N_BODY_SIMULATION_SEQUENTIALSIMULATION_H