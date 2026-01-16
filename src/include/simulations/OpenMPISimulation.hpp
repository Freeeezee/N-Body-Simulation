#ifndef N_BODY_SIMULATION_OPENMPISIMULATION_HPP
#define N_BODY_SIMULATION_OPENMPISIMULATION_HPP

#include "Simulation.hpp"

class OpenMpiSimulation : public Simulation {
public:
    OpenMpiSimulation(float timeStep, const std::vector<Body>& bodies);
    std::vector<Body> calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPISIMULATION_HPP