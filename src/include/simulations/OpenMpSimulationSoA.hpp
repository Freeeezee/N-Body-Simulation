#ifndef N_BODY_SIMULATION_OPENMPSIMULATIONSOA_HPP
#define N_BODY_SIMULATION_OPENMPSIMULATIONSOA_HPP
#include "Simulation.hpp"

class OpenMpSimulationSoA : public SimulationSoA {
public:
    OpenMpSimulationSoA(const float timeStep, const std::vector<Body>& bodies)
        : SimulationSoA(timeStep, bodies) {}

    BodiesSoA calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSIMULATIONSOA_HPP