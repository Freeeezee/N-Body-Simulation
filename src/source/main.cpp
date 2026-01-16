#include "generateBodies.h"
#include "runSimulation.hpp"
#include "../include/simulations/Simulation.hpp"
#include "rendering/Renderer.hpp"
#include "simulations/OpenClSimulation.hpp"
#include "simulations/OpenMpSimulation.hpp"
#include "simulations/OpenMpSingleLoopSimulation.hpp"

int main() {
    const auto bodies = generateBodies(
        1000,
        1.0f,
        1.0f,
        {-200.0f, -200.0f, -200.0f},
        {200.0f, 200.0f, 200.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {
            Body(1.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}),
        });

    const auto simulation = new OpenMpSingleLoopSimulation(1.0f, bodies);

    std::vector<std::vector<Body>> simulationResults = {};

    runSimulation(&simulationResults, simulation, 750);

    playSimulationResults(&simulationResults);
}
