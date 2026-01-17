#ifndef N_BODY_SIMULATION_RUNSIMULATION_H
#define N_BODY_SIMULATION_RUNSIMULATION_H
#include "simulations/Simulation.hpp"
#include "models/Body.hpp"

void runSimulation(std::vector<std::vector<Body>> * buffer, Simulation * simulation, size_t steps);
void runSimulation(std::vector<BodiesSoA> * buffer, SimulationSoA * simulation, size_t steps);

void playSimulationResults(const std::vector<std::vector<Body>> * buffer);
void playSimulationResults(const std::vector<BodiesSoA> * buffer);

#endif //N_BODY_SIMULATION_RUNSIMULATION_H