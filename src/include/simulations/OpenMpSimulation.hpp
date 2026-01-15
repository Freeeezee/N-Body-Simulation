#ifndef N_BODY_SIMULATION_OPENMPSIMULATION_HPP
#define N_BODY_SIMULATION_OPENMPSIMULATION_HPP
#include "Simulation.hpp"

class OpenMpSimulation : public Simulation {
public:
    OpenMpSimulation(const float timeStep, const std::vector<Body>& bodies)
        : Simulation(timeStep, bodies) {}

    std::vector<Body> calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSIMULATION_HPP