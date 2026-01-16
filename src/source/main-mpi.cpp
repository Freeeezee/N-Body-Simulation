#include <iostream>
#include <mpi.h>

#include "simulations/MpiSimulation.hpp"
#include "util/fileUtil.hpp"
#include "util/runSimulation.hpp"
#include "util/serializeString.hpp"

int main(int argc, char ** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const auto serializedBodies = loadFromFile("generated_bodies.txt");
    const auto bodies = deserializeStringBodies(serializedBodies);

    const auto simulation = new MpiSimulation(1.0f, bodies);

    std::vector<std::vector<Body>> simulationResults = {};

    runSimulation(&simulationResults, simulation, 750);

    if (world_rank == 0) {
        std::cout << "Simulation finished. Launching renderer..." << std::endl;
        playSimulationResults(&simulationResults);
    }

    delete simulation;
    MPI_Finalize();
    return 0;
}
