#ifndef N_BODY_SIMULATION_OPENMPOPENCLSIMULATIONSOA1_HPP
#define N_BODY_SIMULATION_OPENMPOPENCLSIMULATIONSOA1_HPP

#include "Simulation.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class OpenClSimulationSoA1 : public SimulationSoA1 {
public:
    OpenClSimulationSoA1(float timeStep, const std::vector<Body>& bodies);
    ~OpenClSimulationSoA1() override;
    BodiesSoA1 calculateNextTick() override;

private:
    cl_device_id deviceId{};
    cl_context context{};
    cl_command_queue queue{};
    cl_program program{};
    cl_kernel kernel{};

    cl_mem bufPosX{};
    cl_mem bufPosY{};
    cl_mem bufPosZ{};
    cl_mem bufMass{};

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

#endif //N_BODY_SIMULATION_OPENMPOPENCLSIMULATIONSOA1_HPP