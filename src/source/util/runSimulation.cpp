#include "util/runSimulation.hpp"

#include <iostream>

#include "OpenGLFrameworkException.hpp"
#include "util/stopwatch.hpp"
#include "rendering/Renderer.hpp"
#include "util/bodies.h"

void runSimulation(std::vector<std::vector<Body>> *buffer, Simulation *simulation, const size_t steps) {
    std::cout << "Starting simulation with " << steps << " steps..." << std::endl;

    stopwatch sw;
    sw.start();

    for (size_t i = 0; i < steps; ++i) {
        buffer->push_back(simulation->calculateNextTick());
    }

    sw.stop();

    std::cout << "Finished simulation" << std::endl;
    std::cout << "Total time: " << sw.elapsedMillis() << " ms" << std::endl;
    std::cout << "Average time per step: " << sw.elapsedMillis() / steps << " ms" << std::endl << std::endl;
}

void runSimulation(std::vector<BodiesSoA> *buffer, SimulationSoA *simulation, const size_t steps) {
    std::cout << "Starting simulation with " << steps << " steps..." << std::endl;

    stopwatch sw;
    sw.start();

    for (size_t i = 0; i < steps; ++i) {
        buffer->push_back(simulation->calculateNextTick());
    }

    sw.stop();

    std::cout << "Finished simulation" << std::endl;
    std::cout << "Total time: " << sw.elapsedMillis() << " ms" << std::endl;
    std::cout << "Average time per step: " << sw.elapsedMillis() / steps << " ms" << std::endl << std::endl;
}

void playSimulationResults(const std::vector<std::vector<Body>> *buffer) {
    std::cout << "Playing back simulation results..." << std::endl;

    try {
        const auto renderer = new Renderer();

        for (const auto& bodies : *buffer) {
            if (!renderer->isWindowOpen()) {
                break;
            }
            renderer->Draw(bodies);
        }
    } catch (OpenGLFrameworkException &exception) {
        std::cerr << exception.what() << std::endl;
    }
}

void playSimulationResults(const std::vector<BodiesSoA> *buffer) {
    std::cout << "Playing back simulation results..." << std::endl;

    try {
        const auto renderer = new Renderer();

        for (const auto& bodies : *buffer) {
            if (!renderer->isWindowOpen()) {
                break;
            }
            renderer->Draw(convertSoAToBodies(bodies));
        }
    } catch (OpenGLFrameworkException &exception) {
        std::cerr << exception.what() << std::endl;
    }
}