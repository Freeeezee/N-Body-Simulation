#include "util/TestSuite.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"
#include "util/stopwatch.hpp"
#include "util/comparison.hpp"
#include <iostream>
#include <fstream>
#include <map>

TestSuite::TestSuite(const std::string &bodiesFile, const float dt) : timeStep(dt) {
    if (const std::string content = loadFromFile(bodiesFile); content.empty()) {
        std::cerr << "Error: Could not load bodies from " << bodiesFile << std::endl;
        initialBodies = {};
    } else {
        initialBodies = deserializeStringBodies(content);
        std::cout << "Loaded " << initialBodies.size() << " bodies for testing." << std::endl;
    }
}

void TestSuite::runAll(const int steps, const int repetitions) const {
    std::cout << "\n================================================\n";
    std::cout << "Running Test Suite: " << initialBodies.size() << " bodies, " << steps << " steps\n";
    std::cout << "================================================" << std::endl;

    const int bodySize = initialBodies.size();
    std::vector<std::string> analyticsRows;
    analyticsRows.push_back("Simulation,Repetition,TotalTime(ms),AvgStep(ms),MSE_vs_Baseline,Status");

    // Store baseline results for quality checks
    std::map<int, std::vector<Body> > baselineResults;

    // 1. Identify and Run Baseline First (if exists)
    for (const auto &config: configs) {
        if (!config.isBaseline) continue;

        runWarmUp(config);
        std::cout << "Running Baseline: " << config.name << "..." << std::endl;

        // We only really need to run the baseline once to get the "correct" numbers
        // for quality checks, but we run 'repetitions' times for timing consistency.
        for (int r = 0; r < repetitions; ++r) {
            {
                stopwatch sw; //
                const std::unique_ptr<ISimRunner> runner(config.factory(initialBodies));

                sw.start();
                for (int s = 0; s < steps; ++s) {
                    runner->step();
                }
                sw.stop();

                auto finalState = runner->getCurrentState();

                const long long time = sw.elapsedMillis();
                analyticsRows.push_back(
                    config.name + "," + std::to_string(r) + "," +
                    std::to_string(time) + "," +
                    std::to_string(static_cast<double>(time) / steps) + ",0.0,Baseline"
                );

                // Store the first run as the "truth"
                if (r == 0) {
                    baselineResults[0] = finalState;
                    saveResultToFile(config.name, bodySize, steps, finalState);
                    std::cout << "  reps done:";
                }
            }
            std::cout << " " + std::to_string(r);
        }
        std::cout << "\n  " << config.name << " complete.\n" << std::endl;
    }

    // 2. Run all other simulations
    for (const auto &config: configs) {
        if (config.isBaseline) continue;

        runWarmUp(config);

        std::cout << "Testing: " << config.name << "..." << std::endl;
        for (int r = 0; r < repetitions; ++r) {
            {
                stopwatch sw;
                const std::unique_ptr<ISimRunner> runner(config.factory(initialBodies));

                sw.start();
                for (int s = 0; s < steps; ++s) {
                    runner->step();
                }
                sw.stop();

                auto finalState = runner->getCurrentState();

                // Quality Check against Baseline
                double mse = 0.0;
                std::string status = "OK";

                if (baselineResults.count(0)) {
                    const auto metrics = calculateQualityMetrics(baselineResults[0], finalState);
                    mse = metrics.meanSquaredError;
                    if (metrics.isDivergent) status = "DIVERGENT";
                } else {
                    status = "NO_BASELINE";
                }

                const long long time = sw.elapsedMillis();
                analyticsRows.push_back(
                    config.name + "," + std::to_string(r) + "," +
                    std::to_string(time) + "," +
                    std::to_string(static_cast<double>(time) / steps) + "," +
                    std::to_string(mse) + "," + status
                );

                if (status == "DIVERGENT") {
                    std::cerr << "  [WARNING] " << config.name << " diverted heavily! MSE: " << mse << std::endl;
                }

                // Save results for the first repetition
                if (r == 0) {
                    saveResultToFile(config.name, bodySize, steps, finalState);
                    std::cout << "  reps done:";
                }
            }
            std::cout << " " + std::to_string(r);
        }
        std::cout << "\n  " << config.name << " complete.\n" << std::endl;
    }

    saveAnalytics("test_suite_analytics_" + std::to_string(steps) + "steps_" + std::to_string(bodySize) + "bodies.csv",
                  analyticsRows);
}

void TestSuite::runWarmUp(const SimulationConfig &config) const {
    std::cout << "Warming up " << config.name << "...";
    {
        // Create a temporary instance just to warm up CPU caches and branch predictors
        const std::unique_ptr<ISimRunner> warmUpRunner(config.factory(initialBodies));
        for (int i = 0; i < 10; ++i) warmUpRunner->step();
    }
    std::cout << "done" << std::endl;
}

void TestSuite::saveAnalytics(const std::string &filename, const std::vector<std::string> &csvRows) {
    std::string content;
    for (const auto &row: csvRows) {
        content += row + "\n";
    }
    saveToFile(filename, content);
    std::cout << "Analytics saved to " << filename << std::endl;
}

void TestSuite::saveResultToFile(const std::string &simName, const int bodySize, const int stepCount,
                                 const std::vector<Body> &bodies) {
    const std::string filename = "results_" + simName + "_" + std::to_string(stepCount) + "steps_" +
                                 std::to_string(bodySize) + "bodies.txt";
    const std::string content = serializeStringBodies(&bodies);
    saveToFile(filename, content);
    std::cout << "  Saved final state to " << filename << std::endl;
}
