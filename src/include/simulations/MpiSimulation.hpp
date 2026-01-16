#ifndef N_BODY_SIMULATION_MPISIMULATION_HPP
#define N_BODY_SIMULATION_MPISIMULATION_HPP

#include "Simulation.hpp"

class MpiSimulation : public Simulation {
public:
    MpiSimulation(const float timeStep, const std::vector<Body>& bodies) : Simulation(timeStep, bodies) {}
    std::vector<Body> calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_MPISIMULATION_HPP