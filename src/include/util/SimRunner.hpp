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

// Adapter for SoA 1 Simulation
class SoARunner1 : public ISimRunner {
    std::unique_ptr<SimulationSoA1> sim;
    BodiesSoA1 lastState;
public:
    explicit SoARunner1(SimulationSoA1* s) : sim(s) {}

    void step() override {
        lastState = sim->calculateNextTick();
    }

    std::vector<Body> getCurrentState() override {
        return convertSoA1ToBodies(lastState);
    }
};

// Adapter for SoA 2 Simulation
class SoARunner2 : public ISimRunner {
    std::unique_ptr<SimulationSoA2> sim;
    BodiesSoA2 lastState;
public:
    explicit SoARunner2(SimulationSoA2* s) : sim(s) {}

    void step() override {
        lastState = sim->calculateNextTick();
    }

    std::vector<Body> getCurrentState() override {
        return convertSoA2ToBodies(lastState);
    }
};

#endif