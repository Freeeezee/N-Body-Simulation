#include <iostream>
#include <mpi.h>
#include <vector>

#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"
#include "util/runSimulation.hpp"

#include "simulations/OpenMpSimulationAoS.hpp"
#include "simulations/SequentialSimulationAoS.hpp"
#include "util/stopwatch.hpp"

using ChosenSimulation = SequentialSimulationAoS;
#define STEPS 500

static void computeChunk(const int worldRank, const int worldSize, const size_t n,
                         size_t& startIndex, size_t& endIndexExclusive) {
    const size_t chunkSize = n / static_cast<size_t>(worldSize);
    startIndex = static_cast<size_t>(worldRank) * chunkSize;
    endIndexExclusive = worldRank == worldSize - 1 ? n : startIndex + chunkSize;
}

int main(int argc, char** argv) {
    int provided = 0;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int worldRank = 0;
    int worldSize = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    std::vector<Body> bodies;
    if (worldRank == 0) {
        const auto serializedBodies = loadFromFile("generated_bodies.txt");
        bodies = deserializeStringBodies(serializedBodies);
    }

    size_t n = bodies.size();
    MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    if (worldRank != 0) {
        bodies.resize(n);
    }

    constexpr float dt = 1.0f;
    ChosenSimulation simulation(dt, bodies);

    std::vector<int> recvCounts(worldSize);
    std::vector<int> displs(worldSize);
    for (int r = 0; r < worldSize; ++r) {
        size_t s = 0, e = 0;
        computeChunk(r, worldSize, n, s, e);
        const size_t countBodies = e - s;
        recvCounts[r] = static_cast<int>(countBodies * sizeof(Body));
        displs[r] = static_cast<int>(s * sizeof(Body));
    }

    size_t startIndex = 0, endIndexExclusive = 0;
    computeChunk(worldRank, worldSize, n, startIndex, endIndexExclusive);
    const size_t localCount = endIndexExclusive - startIndex;

    std::vector<Body> localNext(localCount);

    std::vector<std::vector<Body>> simulationResults;
    if (worldRank == 0) {
        simulationResults.reserve(STEPS);
    }

    simulation.setRange(startIndex, endIndexExclusive);

    stopwatch sw;
    sw.start();

    for (size_t step = 0; step < STEPS; ++step) {
        MPI_Bcast(bodies.data(), static_cast<int>(n * sizeof(Body)), MPI_BYTE, 0, MPI_COMM_WORLD);

        simulation.setBodies(bodies);
        const std::vector<Body> fullNext = simulation.calculateNextTick();

        for (size_t k = 0; k < localCount; ++k) {
            localNext[k] = fullNext[startIndex + k];
        }

        MPI_Allgatherv(
            localNext.data(), static_cast<int>(localCount * sizeof(Body)), MPI_BYTE,
            bodies.data(), recvCounts.data(), displs.data(), MPI_BYTE,
            MPI_COMM_WORLD
        );

        if (worldRank == 0) {
            simulationResults.push_back(bodies);
        }
    }

    sw.stop();

    if (worldRank == 0) {
        std::cout << "Simulation finished." << std::endl;

        std::cout << "Total time: " << sw.elapsedMillis() << " ms" << std::endl;
        std::cout << "Average time per step: " << sw.elapsedMillis() / STEPS << " ms" << std::endl << std::endl;

        std::cout << "Launching renderer..." << std::endl;
        playSimulationResults(&simulationResults);
    }

    MPI_Finalize();
    return 0;
}
