#ifndef N_BODY_SIMULATION_TESTSUITE_H
#define N_BODY_SIMULATION_TESTSUITE_H

#include <string>
#include <vector>
#include <functional>
#include "models/Body.hpp"
#include "SimRunner.hpp"

struct SimulationConfig {
    std::string name;
    // Factory function to create a new instance for every run
    std::function<ISimRunner*(const std::vector<Body>&)> factory;
    bool isBaseline; // If true, other sims are compared against this one
};

class TestSuite {
public:
    TestSuite(const std::string& bodiesFile, float dt);

    // Register a simulation implementation (AoS style)
    template <typename T>
    void registerSimulation(const std::string& name, bool isBaseline = false) {
        configs.push_back({
            name,
            [this](const std::vector<Body>& b) {
                return new AoSRunner(new T(this->timeStep, b));
            },
            isBaseline
        });
    }

    // Register a simulation implementation (SoA style)
    template <typename T>
    void registerSoASimulation(const std::string& name) {
        configs.push_back({
            name,
            [this](const std::vector<Body>& b) {
                return new SoARunner(new T(this->timeStep, b));
            },
            false
        });
    }

    void runAll(int steps, int repetitions) const;

private:
    float timeStep;
    std::vector<Body> initialBodies;
    std::vector<SimulationConfig> configs;

    void runWarmUp(const SimulationConfig &config) const;

    static void saveAnalytics(const std::string& filename, const std::vector<std::string>& csvRows);

    static void saveResultToFile(const std::string& simName, const int bodySize, const int stepCount, const std::vector<Body>& bodies);
};

#endif //N_BODY_SIMULATION_TESTSUITE_H