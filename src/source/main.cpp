#include "util/bodies.h"
#include "util/runSimulation.hpp"
#include "rendering/Renderer.hpp"
#include "simulations/OpenClSimulationSoA.hpp"
#include "simulations/OpenMpSimulationSoA.hpp"
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

    const auto simulation = new OpenClSimulationSoA(1.0f, bodies);

    //std::vector<std::vector<Body>> simulationResults = {};
    std::vector<BodiesSoA> simulationResults = {};

    runSimulation(&simulationResults, simulation, 750);

    playSimulationResults(&simulationResults);
}
