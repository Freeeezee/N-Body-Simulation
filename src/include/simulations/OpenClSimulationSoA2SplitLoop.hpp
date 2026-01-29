#ifndef N_BODY_SIMULATION_OPENCLSIMULATIONSOA2SPLITLOOP_H
#define N_BODY_SIMULATION_OPENCLSIMULATIONSOA2SPLITLOOP_H

#include "Simulation.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class OpenClSimulationSoA2SplitLoop : public SimulationSoA2 {
public:
    OpenClSimulationSoA2SplitLoop(float timeStep, const std::vector<Body>& bodies);
    ~OpenClSimulationSoA2SplitLoop() override;
    BodiesSoA2 calculateNextTick() override;

private:
    cl_device_id deviceId{};
    cl_context context{};
    cl_command_queue queue{};
    cl_program program{};

    cl_kernel kernelGMass{};
    cl_kernel kernelForces{};

    cl_mem bufPosX{};
    cl_mem bufPosY{};
    cl_mem bufPosZ{};
    cl_mem bufMass{};
    cl_mem bufGMass{};

    cl_mem bufVelX{};
    cl_mem bufVelY{};
    cl_mem bufVelZ{};

    cl_mem bufNewPosX{};
    cl_mem bufNewPosY{};
    cl_mem bufNewPosZ{};

    cl_mem bufNewVelX{};
    cl_mem bufNewVelY{};
    cl_mem bufNewVelZ{};

    void initOpenCL();
    void updateBuffers();
};

#endif //N_BODY_SIMULATION_OPENCLSIMULATIONSOA2SPLITLOOP_H