#ifndef N_BODY_SIMULATION_OPENMPSIMULATIONSOA1_HPP
#define N_BODY_SIMULATION_OPENMPSIMULATIONSOA1_HPP
#include "Simulation.hpp"

class OpenMpSimulationSoA1 : public SimulationSoA1 {
public:
    OpenMpSimulationSoA1(const float timeStep, const std::vector<Body>& bodies)
        : SimulationSoA1(timeStep, bodies) {}

    BodiesSoA1 calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSIMULATIONSOA1_HPP