#ifndef N_BODY_SIMULATION_OPENMPSINGLELOOPSIMULATION_HPP
#define N_BODY_SIMULATION_OPENMPSINGLELOOPSIMULATION_HPP
#include "Simulation.hpp"

class OpenMpSingleLoopSimulation : public Simulation {
public:
    OpenMpSingleLoopSimulation(const float timeStep, const std::vector<Body>& bodies)
        : Simulation(timeStep, bodies) {}

    std::vector<Body> calculateNextTick() override;
};

#endif //N_BODY_SIMULATION_OPENMPSINGLELOOPSIMULATION_HPP