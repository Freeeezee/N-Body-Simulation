#include "../include/simulations/OpenClSimulation.hpp"
#include "../include/simulations/SequentialSimulation.hpp"
#include "../include/util/TestSuite.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"
#include <vector>
#include <string>
#include <iostream>

#include "simulations/MpiSimulation.hpp"
#include "simulations/OpenMpSimulation.hpp"
#include "simulations/OpenMpSingleLoopSimulation.hpp"


void generateTestSets(const std::vector<int> &testCounts) {

    constexpr float mass = 1.0f;
    constexpr float radius = 1.0f;
    constexpr glm::vec3 minPos = { -200.0f, -200.0f, -200.0f };
    constexpr glm::vec3 maxPos = { 200.0f, 200.0f, 200.0f };
    constexpr glm::vec3 minVel = { -1.0f, -1.0f, -1.0f };
    constexpr glm::vec3 maxVel = { 1.0f, 1.0f, 1.0f };

    const std::vector initialBodies = {
        Body(1.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f})
    };

    // Generation
    for (int count : testCounts) {
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
        saveToFile(filename.c_str(), serialized);

        std::cout << "Saved to " << filename << std::endl;
    }
}

int main(int argc, char** argv) {
    const std::vector bodyCounts = { 2, 3, 10, 50, 100, 500, 1000, 5000, 10000 };
    const std::vector testStepCounts = { 10, 50, 100, 500, 1000, 5000 };
    constexpr int repetitions = 5;

    generateTestSets(bodyCounts);

    for (const int count : bodyCounts) {
        std::string filename = "generated_bodies" + std::to_string(count) + ".txt";
        TestSuite suite(filename, 1.0f);
        suite.registerSimulation<SequentialSimulation>("Sequential_Baseline", true);
        suite.registerSoASimulation<OpenClSimulation>("OpenClSimulation");
        suite.registerSoASimulation<OpenMpSimulation>("OpenMpSimulation");
        //suite.registerSimulation<OpenMpSingleLoopSimulation>("OpenMpSingleLoopSimulation");
        //suite.registerSimulation<MpiSimulation>("MpiSimulation");
        for (const int stepCount : testStepCounts) {
            suite.runAll(stepCount, repetitions);
        }
    }

    return 0;
}