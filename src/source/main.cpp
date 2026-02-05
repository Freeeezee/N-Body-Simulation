#include "util/bodies.h"
#include "util/runSimulation.hpp"
#include "rendering/Renderer.hpp"
#include "simulations/OpenClSimulationSoA2.hpp"
#include "simulations/OpenClSimulationSoA2SplitLoop.hpp"
#include "simulations/OpenClSimulationSoA2SplitLoopTiled.hpp"
#include "simulations/OpenMpSimulationSoA2.hpp"
#include "simulations/OpenMpSimulationSoA2SplitLoop.hpp"
#include "simulations/OpenMpSingleLoopSimulation.hpp"
#include "simulations/SequentialSimulationAoS.hpp"
#include "simulations/SequentialSimulationSoA2.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"

int main() {
    const auto bodies = generateBodies(
        16384,
        1.0f,
        1.0f,
        {-200.0f, -200.0f, -200.0f},
        {200.0f, 200.0f, 200.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f});

    // const auto serializedBodies = loadFromFile("generated_bodies.txt");
    // const auto bodies = deserializeStringBodies(serializedBodies);

    const auto simulation = new OpenClSimulationSoA2SplitLoopTiled(1.0f, bodies);

    //std::vector<std::vector<Body>> simulationResults = {};
    std::vector<BodiesSoA2> simulationResults = {};

    runSimulation(&simulationResults, simulation, 500);

    playSimulationResults(&simulationResults);
}
