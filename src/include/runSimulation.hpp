#ifndef N_BODY_SIMULATION_RUNSIMULATION_H
#define N_BODY_SIMULATION_RUNSIMULATION_H
#include "simulations/Simulation.hpp"
#include "models/Body.hpp"

void runSimulation(std::vector<std::vector<Body>> * buffer, Simulation * simulation, size_t steps);

void playSimulationResults(std::vector<std::vector<Body>> * buffer);

#endif //N_BODY_SIMULATION_RUNSIMULATION_H