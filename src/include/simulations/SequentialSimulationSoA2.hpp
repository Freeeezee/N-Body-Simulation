#ifndef N_BODY_SIMULATION_SEQUENTIALSIMULATIONSOA2_HPP
#define N_BODY_SIMULATION_SEQUENTIALSIMULATIONSOA2_HPP
#include "Simulation.hpp"

class SequentialSimulationSoA2 : public SimulationSoA2 {
public:
    SequentialSimulationSoA2(const float timeStep, const std::vector<Body>& bodies)
        : SimulationSoA2(timeStep, bodies) {}

    BodiesSoA2 calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_SEQUENTIALSIMULATIONSOA2_HPP