#ifndef N_BODY_SIMULATION_OPENMPSIMULATION_HPP
#define N_BODY_SIMULATION_OPENMPSIMULATION_HPP
#include "Simulation.hpp"

class OpenMpSimulation : public SimulationSoA {
public:
    OpenMpSimulation(const float timeStep, const std::vector<Body>& bodies)
        : SimulationSoA(timeStep, bodies) {}

    BodiesSoA calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSIMULATION_HPP