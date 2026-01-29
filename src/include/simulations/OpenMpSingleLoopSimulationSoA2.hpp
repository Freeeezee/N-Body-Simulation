#ifndef N_BODY_SIMULATION_OPENMPSINGLELOOPSIMULATIONSOA2_HPP
#define N_BODY_SIMULATION_OPENMPSINGLELOOPSIMULATIONSOA2_HPP

#include "simulations/Simulation.hpp"

class OpenMpSingleLoopSimulationSoA2 : public SimulationSoA2 {
public:
    OpenMpSingleLoopSimulationSoA2(const float timeStep, const std::vector<Body>& bodies)
        : SimulationSoA2(timeStep, bodies) {}

    BodiesSoA2 calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSINGLELOOPSIMULATIONSOA2_HPP