#include "simulations/OpenClSimulationAoS.hpp"
#include "Constants.hpp"
#include <fstream>
#include <iostream>
#include <vector>

OpenClSimulationAoS::OpenClSimulationAoS(const float timeStep, const std::vector<Body> &bodies)
    : Simulation(timeStep, bodies) {
    initOpenCL();
    updateBuffers();
}

void OpenClSimulationAoS::initOpenCL() {
    cl_uint num_platforms;
    clGetPlatformIDs(1, nullptr, &num_platforms);
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &deviceId, nullptr);
    context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, nullptr);
    queue = clCreateCommandQueue(context, deviceId, 0, nullptr);

    std::ifstream kernelFile("kernels/body-force-calculationAoS.cl");
    std::string src((std::istreambuf_iterator(kernelFile)), std::istreambuf_iterator<char>());
    const char* srcPtr = src.c_str();
    size_t srcSize = src.length();

    program = clCreateProgramWithSource(context, 1, &srcPtr, &srcSize, nullptr);

    const auto buildOptions = "";

    if (const cl_int buildErr = clBuildProgram(program, 1, &deviceId, buildOptions, nullptr, nullptr);
        buildErr != CL_SUCCESS) {
        size_t logSize = 0;
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

        std::string log(logSize, '\0');
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

        std::cerr << "OpenCL build failed (" << buildErr << ")\n";
        std::cerr << "Build log:\n" << log << "\n";
        throw std::runtime_error("clBuildProgram failed");
    }

    kernel = clCreateKernel(program, "calculate_forces", nullptr);

    const size_t size = bodies.size() * sizeof(cl_float4);
    bufPosMass = clCreateBuffer(context, CL_MEM_READ_WRITE, size, nullptr, nullptr);
    bufVel = clCreateBuffer(context, CL_MEM_READ_WRITE, size, nullptr, nullptr);
    bufNewPos = clCreateBuffer(context, CL_MEM_READ_WRITE, size, nullptr, nullptr);
    bufNewVel = clCreateBuffer(context, CL_MEM_READ_WRITE, size, nullptr, nullptr);
}

void OpenClSimulationAoS::updateBuffers() {
    std::vector<cl_float4> posMass(bodies.size());
    std::vector<cl_float4> vel(bodies.size());

    for (size_t i = 0; i < bodies.size(); ++i) {
        posMass[i] = {bodies[i].position.x, bodies[i].position.y, bodies[i].position.z, bodies[i].mass};
        vel[i] = {bodies[i].velocity.x, bodies[i].velocity.y, bodies[i].velocity.z, 0.0f};
    }

    clEnqueueWriteBuffer(queue, bufPosMass, CL_TRUE, 0, posMass.size() * sizeof(cl_float4), posMass.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufVel, CL_TRUE, 0, vel.size() * sizeof(cl_float4), vel.data(), 0, nullptr, nullptr);
}

std::vector<Body> OpenClSimulationAoS::calculateNextTick() {
    int numBodies = static_cast<int>(bodies.size());

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufPosMass);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufVel);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufNewPos);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufNewVel);
    clSetKernelArg(kernel, 4, sizeof(float), &timeStep);
    constexpr float g_val = G;
    constexpr float eps_val = DISTANCE_EPSILON;
    clSetKernelArg(kernel, 5, sizeof(float), &g_val);
    clSetKernelArg(kernel, 6, sizeof(float), &eps_val);
    clSetKernelArg(kernel, 7, sizeof(int), &numBodies);

    size_t globalSize = numBodies;
    clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);

    std::vector<cl_float4> resPos(numBodies);
    std::vector<cl_float4> resVel(numBodies);
    clEnqueueReadBuffer(queue, bufNewPos, CL_TRUE, 0, numBodies * sizeof(cl_float4), resPos.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewVel, CL_TRUE, 0, numBodies * sizeof(cl_float4), resVel.data(), 0, nullptr, nullptr);

    for (size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].position = {resPos[i].s[0], resPos[i].s[1], resPos[i].s[2]};
        bodies[i].velocity = {resVel[i].s[0], resVel[i].s[1], resVel[i].s[2]};
    }

    std::swap(bufPosMass, bufNewPos);
    std::swap(bufVel, bufNewVel);

    return bodies;
}

OpenClSimulationAoS::~OpenClSimulationAoS() {
    clReleaseMemObject(bufPosMass);
    clReleaseMemObject(bufVel);
    clReleaseMemObject(bufNewPos);
    clReleaseMemObject(bufNewVel);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
