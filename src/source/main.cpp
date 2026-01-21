#include "util/bodies.h"
#include "util/runSimulation.hpp"
#include "rendering/Renderer.hpp"
#include "simulations/OpenClSimulation.hpp"
#include "simulations/OpenMpSimulation.hpp"
#include "simulations/OpenMpSingleLoopSimulation.hpp"

int main() {
    const auto bodies = generateBodies(
        10000,
        1.0f,
        1.0f,
        {-200.0f, -200.0f, -200.0f},
        {200.0f, 200.0f, 200.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {
            Body(1.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}),
        });

    const auto simulation = new OpenClSimulation(1.0f, bodies);

    //std::vector<std::vector<Body>> simulationResults = {};
    std::vector<BodiesSoA> simulationResults = {};

    runSimulation(&simulationResults, simulation, 750);

    playSimulationResults(&simulationResults);
}
