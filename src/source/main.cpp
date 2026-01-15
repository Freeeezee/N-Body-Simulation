#include <iostream>

#include "generateBodies.h"
#include "OpenGLFrameworkException.hpp"
#include "runSimulation.hpp"
#include "Simulation.hpp"
#include "rendering/Renderer.hpp"

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

    const auto simulation = new Simulation(1.0f, bodies);

    std::vector<std::vector<Body>> simulationResults = {};

    runSimulation(&simulationResults, simulation, 100);

    playSimulationResults(&simulationResults);
}
