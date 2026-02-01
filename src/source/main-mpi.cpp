#include <iostream>
#include <mpi.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"
#include "util/runSimulation.hpp"

#include "simulations/OpenMpSimulationAoS.hpp"
#include "simulations/SequentialSimulationAoS.hpp"
#include "util/stopwatch.hpp"

using ChosenSimulation = SequentialSimulationAoS;
#define STEPS 500
#define TIMING_STEPS 5

static void computeChunk(const int worldRank, const int worldSize, const size_t n,
                         size_t& startIndex, size_t& endIndexExclusive) {
    const size_t chunkSize = n / static_cast<size_t>(worldSize);
    startIndex = static_cast<size_t>(worldRank) * chunkSize;
    endIndexExclusive = worldRank == worldSize - 1 ? n : startIndex + chunkSize;
}

static void computeWeightedChunksFromTimes(
    const size_t n,
    const std::vector<double>& avgTimes,
    std::vector<int>& recvCountsBytes,
    std::vector<int>& displsBytes
) {
    const int worldSize = static_cast<int>(avgTimes.size());
    constexpr double eps = 1e-12;

    std::vector speeds(worldSize, 0.0);
    for (int r = 0; r < worldSize; ++r) {
        speeds[r] = 1.0 / std::max(avgTimes[r], eps);
    }
    const double sumSpeed = std::accumulate(speeds.begin(), speeds.end(), 0.0);

    std::vector<size_t> counts(worldSize, 0);
    size_t assigned = 0;
    for (int r = 0; r < worldSize; ++r) {
        const double frac = sumSpeed > 0.0 ? speeds[r] / sumSpeed : 1.0 / worldSize;
        const size_t c = std::llround(frac * static_cast<double>(n));
        counts[r] = c;
        assigned += c;
    }

    if (assigned != n) {
        if (assigned < n) {
            std::vector<int> order(worldSize);
            std::iota(order.begin(), order.end(), 0);
            std::sort(order.begin(), order.end(), [&](int a, int b) { return speeds[a] > speeds[b]; });

            const size_t missing = n - assigned;
            for (size_t i = 0; i < missing; ++i) {
                counts[order[i % static_cast<size_t>(worldSize)]] += 1;
            }
        } else {
            std::vector<int> order(worldSize);
            std::iota(order.begin(), order.end(), 0);
            std::sort(order.begin(), order.end(), [&](int a, int b) { return speeds[a] < speeds[b]; });

            size_t extra = assigned - n;
            for (size_t i = 0; i < extra; ++i) {
                if (const int r = order[i % static_cast<size_t>(worldSize)]; counts[r] > 0) counts[r] -= 1;
                else extra += 1;
            }
        }
    }

    size_t dispBodies = 0;
    for (int r = 0; r < worldSize; ++r) {
        displsBytes[r] = static_cast<int>(dispBodies * sizeof(Body));
        recvCountsBytes[r] = static_cast<int>(counts[r] * sizeof(Body));
        dispBodies += counts[r];
    }
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
        std::cout << "Loaded bodies from file." << std::endl;
    }

    size_t n = bodies.size();
    MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    if (worldRank != 0) {
        std::cout << "Number of bodies (r1): " << n << std::endl;
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
    size_t localCount = endIndexExclusive - startIndex;

    std::vector<Body> localNext(localCount);

    std::vector<std::vector<Body>> simulationResults;
    if (worldRank == 0) {
        simulationResults.reserve(STEPS);
    }

    simulation.setRange(startIndex, endIndexExclusive);

    double computeTimeSum = 0.0;

    stopwatch sw;
    sw.start();

    for (size_t step = 0; step < STEPS; ++step) {
        std::cout << "Step " << step + 1 << std::endl;
        MPI_Bcast(bodies.data(), static_cast<int>(n * sizeof(Body)), MPI_BYTE, 0, MPI_COMM_WORLD);

        simulation.setBodies(bodies);

        const double t0 = MPI_Wtime();
        const std::vector<Body> fullNext = simulation.calculateNextTick();
        const double t1 = MPI_Wtime();

        if (step < TIMING_STEPS) {
            computeTimeSum += (t1 - t0);
        }

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

        if (step + 1 == TIMING_STEPS) {
            const double avgLocal = computeTimeSum / static_cast<double>(TIMING_STEPS);
            std::vector<double> avgTimes;
            if (worldRank == 0) avgTimes.resize(worldSize);

            MPI_Gather(&avgLocal, 1, MPI_DOUBLE,
                       worldRank == 0 ? avgTimes.data() : nullptr, 1, MPI_DOUBLE,
                       0, MPI_COMM_WORLD);

            if (worldRank == 0) {
                computeWeightedChunksFromTimes(n, avgTimes, recvCounts, displs);
            }

            MPI_Bcast(recvCounts.data(), worldSize, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(displs.data(), worldSize, MPI_INT, 0, MPI_COMM_WORLD);

            startIndex = static_cast<size_t>(displs[worldRank] / static_cast<int>(sizeof(Body)));
            localCount = static_cast<size_t>(recvCounts[worldRank] / static_cast<int>(sizeof(Body)));
            endIndexExclusive = startIndex + localCount;

            localNext.resize(localCount);
            std::cout << "Updating to startindex (R" << worldRank << "): " << startIndex << " endindex: " << endIndexExclusive << std::endl;
            simulation.setRange(startIndex, endIndexExclusive);
        }
    }

    sw.stop();

    if (worldRank == 0) {
        std::cout << "Simulation finished." << std::endl;

        std::cout << "Total time: " << sw.elapsedMillis() << " ms" << std::endl;
        std::cout << "Average time per step: " << sw.elapsedMillis() / STEPS << " ms" << std::endl << std::endl;
    }

    MPI_Finalize();
    return 0;
}
