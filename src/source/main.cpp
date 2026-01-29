#include "util/bodies.h"
#include "util/runSimulation.hpp"
#include "rendering/Renderer.hpp"
#include "simulations/OpenClSimulationSoA2.hpp"
#include "simulations/OpenMpSingleLoopSimulation.hpp"
#include "simulations/SequentialSimulation.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"

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

    // const auto serializedBodies = loadFromFile("generated_bodies.txt");
    // const auto bodies = deserializeStringBodies(serializedBodies);

    const auto simulation = new OpenClSimulationSoA2(1.0f, bodies);

    // std::vector<std::vector<Body>> simulationResults = {};
    std::vector<BodiesSoA2> simulationResults = {};

    runSimulation(&simulationResults, simulation, 750);

    playSimulationResults(&simulationResults);
}
