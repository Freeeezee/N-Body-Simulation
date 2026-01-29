#ifndef N_BODY_SIMULATION_RUNSIMULATION_H
#define N_BODY_SIMULATION_RUNSIMULATION_H
#include "simulations/Simulation.hpp"
#include "models/Body.hpp"

void runSimulation(std::vector<std::vector<Body>> * buffer, Simulation * simulation, size_t steps);
void runSimulation(std::vector<BodiesSoA1> * buffer, SimulationSoA2 * simulation, size_t steps);
void runSimulation(std::vector<BodiesSoA2> * buffer, SimulationSoA2 * simulation, size_t steps);

void playSimulationResults(const std::vector<std::vector<Body>> * buffer);
void playSimulationResults(const std::vector<BodiesSoA1> * buffer);
void playSimulationResults(const std::vector<BodiesSoA2> * buffer);

#endif //N_BODY_SIMULATION_RUNSIMULATION_H