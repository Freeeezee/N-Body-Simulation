#ifndef N_BODY_SIMULATION_OPENMPSIMULATIONAOS_HPP
#define N_BODY_SIMULATION_OPENMPSIMULATIONAOS_HPP
#include "Simulation.hpp"

class OpenMpSimulationAoS : public Simulation {
public:
    OpenMpSimulationAoS(const float timeStep, const std::vector<Body>& bodies)
        : Simulation(timeStep, bodies) {}

    std::vector<Body> calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSIMULATIONAOS_HPP