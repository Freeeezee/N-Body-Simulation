#ifndef N_BODY_SIMULATION_OPENMPSIMULATIONSOA2_HPP
#define N_BODY_SIMULATION_OPENMPSIMULATIONSOA2_HPP
#include "Simulation.hpp"

class OpenMpSimulationSoA2 : public SimulationSoA2 {
public:
    OpenMpSimulationSoA2(const float timeStep, const std::vector<Body>& bodies)
        : SimulationSoA2(timeStep, bodies) {}

    BodiesSoA2 calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSIMULATIONSOA2_HPP