#ifndef N_BODY_SIMULATION_OPENMPOPENCLSIMULATIONAOS_HPP
#define N_BODY_SIMULATION_OPENMPOPENCLSIMULATIONAOS_HPP

#include "Simulation.hpp"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class OpenClSimulationAoS : public Simulation {
public:
    OpenClSimulationAoS(float timeStep, const std::vector<Body>& bodies);
    ~OpenClSimulationAoS();
    std::vector<Body> calculateNextTick() override;

private:
    cl_device_id device_id{};
    cl_context context{};
    cl_command_queue queue{};
    cl_program program{};
    cl_kernel kernel{};

    cl_mem bufPosMass{};
    cl_mem bufVel{};
    cl_mem bufNewPos{};
    cl_mem bufNewVel{};

    void initOpenCL();
    void updateBuffers();
};

#endif //N_BODY_SIMULATION_OPENMPOPENCLSIMULATIONAOS_HPP