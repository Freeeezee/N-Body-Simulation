#include "../include/simulations/OpenClSimulationSoA2.hpp"
#include "../include/util/TestSuite.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"
#include <vector>
#include <string>
#include <iostream>

#include "simulations/OpenClSimulationAoS.hpp"
#include "simulations/OpenClSimulationSoA1.hpp"
#include "simulations/SequentialSimulationSoA2.hpp"
#include "simulations/OpenClSimulationSoA2SplitLoop.hpp"
#include "simulations/OpenMpSimulationAoS.hpp"
#include "simulations/OpenMpSimulationSoA1.hpp"
#include "simulations/OpenMpSimulationSoA2.hpp"
#include "simulations/OpenMpSimulationSoA2SplitLoop.hpp"
#include "simulations/OpenMpSingleLoopSimulation.hpp"
#include "simulations/OpenMpSingleLoopSimulationSoA2.hpp"
#include "simulations/SequentialSimulationAoS.hpp"


void generateTestSets(const std::vector<int> &testCounts) {
    // --- Body Generation Parameters ---

    constexpr float mass = 1.0f;
    constexpr float radius = 1.0f;
    constexpr glm::vec3 minPos = { -200.0f, -200.0f, -200.0f };
    constexpr glm::vec3 maxPos = { 200.0f, 200.0f, 200.0f };
    constexpr glm::vec3 minVel = { -1.0f, -1.0f, -1.0f };
    constexpr glm::vec3 maxVel = { 1.0f, 1.0f, 1.0f };

    const std::vector<Body> initialBodies = {};

    // --- Body Generation ---
    for (const int count : testCounts) {
        std::string filename = "generated_bodies" + std::to_string(count) + ".txt";

        if (fileExists(filename)) {
            std::cout << "Skipping " << filename << " (File already exists)" << std::endl;
            continue;
        }

        std::cout << "Generating " << count << " bodies..." << std::endl;

        const auto bodies = generateBodies(
            count,
            mass,
            radius,
            minPos,
            maxPos,
            minVel,
            maxVel,
            initialBodies
        );

        auto serialized = serializeStringBodies(&bodies);
        saveToFile(filename, serialized);

        std::cout << "Saved to " << filename << std::endl;
    }
}

int main(int argc, char** argv) {
    // --- Test Configuration ---

    // Number of bodies to generate
    const std::vector bodyCounts = { 90000, 100000, 120000, 140000, 160000, 180000, 200000, 240000, 280000, 320000, 360000, 400000 };

    // Number of Timesteps to calculate
    const std::vector testStepCounts = { 750 };

    // Number of Calculation Repetitions
    constexpr int repetitions = 3;

    generateTestSets(bodyCounts);

    for (const int count : bodyCounts) {
        std::string filename = "generated_bodies" + std::to_string(count) + ".txt";
        TestSuite suite(filename, 1.0f);

        // --- Simulations to Compare ---
        // Simulations can be defined as Baseline to compare results of all other simulations to its result.

        suite.registerSimulation<SequentialSimulationAoS>("Sequential_BaselineAoS", true);
        suite.registerSoA2Simulation<SequentialSimulationSoA2>("SequentialSimulationSoA2");
        suite.registerSimulation<OpenClSimulationAoS>("OpenClSimulationAoS");
        suite.registerSoA1Simulation<OpenClSimulationSoA1>("OpenClSimulationSoA1");
        suite.registerSoA2Simulation<OpenClSimulationSoA2>("OpenClSimulationSoA2");
        suite.registerSoA2Simulation<OpenClSimulationSoA2SplitLoop>("OpenClSimulationSoA2SplitLoop");
        suite.registerSimulation<OpenMpSimulationAoS>("OpenMpSimulationAoS");
        suite.registerSoA1Simulation<OpenMpSimulationSoA1>("OpenMpSimulationSoA1");
        suite.registerSoA2Simulation<OpenMpSimulationSoA2>("OpenMpSimulationSoA2");
        suite.registerSimulation<OpenMpSingleLoopSimulation>("OpenMpSingleLoopSimulation");
        suite.registerSoA2Simulation<OpenMpSingleLoopSimulationSoA2>("OpenMpSingleLoopSimulationSoA2");
        suite.registerSoA2Simulation<OpenMpSimulationSoA2SplitLoop>("OpenMpSimulationSoA2SplitLoop");

        for (const int stepCount: testStepCounts) {
            suite.runAll(stepCount, repetitions);
        }
    }

    return 0;
}
