#include "simulations/OpenMPISimulation.hpp"

OpenMpiSimulation::OpenMpiSimulation(const float timeStep, const std::vector<Body> &bodies) : Simulation(timeStep, bodies) {

}

std::vector<Body> OpenMpiSimulation::calculateNextTick() {

}
