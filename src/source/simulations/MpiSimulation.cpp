#include "simulations/MpiSimulation.hpp"
#include "Constants.hpp"
#include <mpi.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

std::vector<Body> MpiSimulation::calculateNextTick() {
    int worldSize, worldRank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    const size_t n = bodies.size();

    MPI_Bcast(bodies.data(), n * sizeof(Body), MPI_BYTE, 0, MPI_COMM_WORLD);

    const int chunkSize = n / worldSize;
    const int startIndex = worldRank * chunkSize;
    const int endIndex = (worldRank == worldSize - 1) ? n : startIndex + chunkSize;
    const int localCount = endIndex - startIndex;

    std::vector<Body> localNextBodies(localCount);

    #pragma omp parallel for schedule(dynamic)
    for (int i = startIndex; i < endIndex; ++i) {
        int local_i = i - startIndex;
        const glm::vec3 pos = bodies[i].position;
        glm::vec3 vel = bodies[i].velocity;
        glm::vec3 totalAcceleration(0.0f);

        for (size_t j = 0; j < n; ++j) {
            if (i == j) continue;

            const glm::vec3 direction = bodies[j].position - pos;
            const float distanceSquared = glm::dot(direction, direction) + DISTANCE_EPSILON;
            const float invDistCubed = 1.0f / (distanceSquared * std::sqrt(distanceSquared));

            totalAcceleration += G * bodies[j].mass * invDistCubed * direction;
        }

        vel += totalAcceleration * timeStep;
        localNextBodies[local_i].position = pos + vel * timeStep;
        localNextBodies[local_i].velocity = vel;
        localNextBodies[local_i].mass = bodies[i].mass;
    }

    std::vector<int> receivedCounts(worldSize);
    std::vector<int> displacements(worldSize);

    for (int i = 0; i < worldSize; ++i) {
        const int c = n / worldSize;
        receivedCounts[i] = (i == worldSize - 1 ? n - i * c : c) * sizeof(Body);
        displacements[i] = i * c * sizeof(Body);
    }

    MPI_Allgatherv(localNextBodies.data(), localCount * sizeof(Body), MPI_BYTE,
                   bodies.data(), receivedCounts.data(), displacements.data(),
                   MPI_BYTE, MPI_COMM_WORLD);

    return bodies;
}
