#ifndef N_BODY_SIMULATION_SIMRUNNER_H
#define N_BODY_SIMULATION_SIMRUNNER_H

#include "util/bodies.h"
#include <memory>
#include <vector>
#include "simulations/Simulation.hpp"

// Abstract interface for running a simulation
class ISimRunner {
public:
    virtual ~ISimRunner() = default;
    virtual void step() = 0;
    virtual std::vector<Body> getCurrentState() = 0;
};

// Adapter for Standard AoS Simulation
class AoSRunner : public ISimRunner {
    std::unique_ptr<Simulation> sim;
    std::vector<Body> lastState;
public:
    explicit AoSRunner(Simulation* s) : sim(s) {}

    void step() override {
        lastState = sim->calculateNextTick();
    }

    std::vector<Body> getCurrentState() override {
        return lastState;
    }
};

// Adapter for SoA Simulation
class SoARunner : public ISimRunner {
    std::unique_ptr<SimulationSoA> sim;
    BodiesSoA lastState;
public:
    explicit SoARunner(SimulationSoA* s) : sim(s) {}

    void step() override {
        lastState = sim->calculateNextTick();
    }

    std::vector<Body> getCurrentState() override {
        return convertSoAToBodies(lastState);
    }
};

#endif