#include "simulations/OpenClSimulationSoA2SplitLoopTiled.hpp"

#include "Constants.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

OpenClSimulationSoA2SplitLoopTiled::OpenClSimulationSoA2SplitLoopTiled(const float timeStep, const std::vector<Body>& bodies)
    : SimulationSoA2(timeStep, bodies) {
    initOpenCL();
    updateBuffers();
}

void OpenClSimulationSoA2SplitLoopTiled::initOpenCL() {
    cl_platform_id platform{};
    cl_uint numPlatforms = 0;
    clGetPlatformIDs(1, &platform, &numPlatforms);

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &deviceId, nullptr);

    context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, nullptr);
    queue = clCreateCommandQueue(context, deviceId, 0, nullptr);

    std::ifstream kernelFile("kernels/body-force-calculationSoASplitLoopTiled.cl");
    if (!kernelFile) {
        throw std::runtime_error("Failed to open kernel file: kernels/body-force-calculationSoASplitLoopTiled.cl");
    }

    const std::string src((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    const char* srcPtr = src.c_str();
    const size_t srcSize = src.size();

    program = clCreateProgramWithSource(context, 1, &srcPtr, &srcSize, nullptr);

    const char* buildOptions = "";
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

    kernelGMass = clCreateKernel(program, "compute_gmasses", nullptr);
    kernelForces = clCreateKernel(program, "calculate_forces_split", nullptr);

    const size_t n = bodies.masses.size();
    const size_t floatSize = n * sizeof(float);

    bufPosX = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufPosY = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufPosZ = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);

    bufMass = clCreateBuffer(context, CL_MEM_READ_ONLY, floatSize, nullptr, nullptr);
    bufGMass = clCreateBuffer(context, CL_MEM_READ_ONLY, floatSize, nullptr, nullptr);

    bufVelX = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufVelY = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufVelZ = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);

    bufNewPosX = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufNewPosY = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufNewPosZ = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);

    bufNewVelX = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufNewVelY = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufNewVelZ = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);

    clEnqueueWriteBuffer(queue, bufMass, CL_TRUE, 0, floatSize, bodies.masses.data(), 0, nullptr, nullptr);

    const int numBodies = static_cast<int>(n);
    clSetKernelArg(kernelGMass, 0, sizeof(cl_mem), &bufMass);
    clSetKernelArg(kernelGMass, 1, sizeof(cl_mem), &bufGMass);
    constexpr float gVal = G;
    clSetKernelArg(kernelGMass, 2, sizeof(float), &gVal);
    clSetKernelArg(kernelGMass, 3, sizeof(int), &numBodies);

    size_t globalSize = n;
    clEnqueueNDRangeKernel(queue, kernelGMass, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);
    clFinish(queue);
}

void OpenClSimulationSoA2SplitLoopTiled::updateBuffers() {
    const size_t n = bodies.masses.size();
    const size_t floatSize = n * sizeof(float);

    clEnqueueWriteBuffer(queue, bufPosX, CL_TRUE, 0, floatSize, bodies.posX.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufPosY, CL_TRUE, 0, floatSize, bodies.posY.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufPosZ, CL_TRUE, 0, floatSize, bodies.posZ.data(), 0, nullptr, nullptr);

    clEnqueueWriteBuffer(queue, bufVelX, CL_TRUE, 0, floatSize, bodies.velX.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufVelY, CL_TRUE, 0, floatSize, bodies.velY.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufVelZ, CL_TRUE, 0, floatSize, bodies.velZ.data(), 0, nullptr, nullptr);
}

BodiesSoA2 OpenClSimulationSoA2SplitLoopTiled::calculateNextTick() {
    const int numBodies = static_cast<int>(bodies.masses.size());

    clSetKernelArg(kernelForces, 0, sizeof(cl_mem), &bufPosX);
    clSetKernelArg(kernelForces, 1, sizeof(cl_mem), &bufPosY);
    clSetKernelArg(kernelForces, 2, sizeof(cl_mem), &bufPosZ);
    clSetKernelArg(kernelForces, 3, sizeof(cl_mem), &bufGMass);

    clSetKernelArg(kernelForces, 4, sizeof(cl_mem), &bufVelX);
    clSetKernelArg(kernelForces, 5, sizeof(cl_mem), &bufVelY);
    clSetKernelArg(kernelForces, 6, sizeof(cl_mem), &bufVelZ);

    clSetKernelArg(kernelForces, 7, sizeof(cl_mem), &bufNewPosX);
    clSetKernelArg(kernelForces, 8, sizeof(cl_mem), &bufNewPosY);
    clSetKernelArg(kernelForces, 9, sizeof(cl_mem), &bufNewPosZ);

    clSetKernelArg(kernelForces, 10, sizeof(cl_mem), &bufNewVelX);
    clSetKernelArg(kernelForces, 11, sizeof(cl_mem), &bufNewVelY);
    clSetKernelArg(kernelForces, 12, sizeof(cl_mem), &bufNewVelZ);

    clSetKernelArg(kernelForces, 13, sizeof(float), &timeStep);
    constexpr float epsVal = DISTANCE_EPSILON;
    clSetKernelArg(kernelForces, 14, sizeof(float), &epsVal);
    clSetKernelArg(kernelForces, 15, sizeof(int), &numBodies);

    constexpr size_t localSize = 256;

    const size_t localBytes = localSize * sizeof(float);
    clSetKernelArg(kernelForces, 16, localBytes, nullptr);
    clSetKernelArg(kernelForces, 17, localBytes, nullptr);
    clSetKernelArg(kernelForces, 18, localBytes, nullptr);
    clSetKernelArg(kernelForces, 19, localBytes, nullptr);

    const size_t n = static_cast<size_t>(numBodies);
    const size_t globalSize = ((n + localSize - 1) / localSize) * localSize;

    clEnqueueNDRangeKernel(queue, kernelForces, 1, nullptr, &globalSize, &localSize, 0, nullptr, nullptr);

    const size_t floatSize = static_cast<size_t>(numBodies) * sizeof(float);

    clEnqueueReadBuffer(queue, bufNewPosX, CL_TRUE, 0, floatSize, bodies.posX.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewPosY, CL_TRUE, 0, floatSize, bodies.posY.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewPosZ, CL_TRUE, 0, floatSize, bodies.posZ.data(), 0, nullptr, nullptr);

    clEnqueueReadBuffer(queue, bufNewVelX, CL_TRUE, 0, floatSize, bodies.velX.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewVelY, CL_TRUE, 0, floatSize, bodies.velY.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewVelZ, CL_TRUE, 0, floatSize, bodies.velZ.data(), 0, nullptr, nullptr);

    std::swap(bufPosX, bufNewPosX);
    std::swap(bufPosY, bufNewPosY);
    std::swap(bufPosZ, bufNewPosZ);

    std::swap(bufVelX, bufNewVelX);
    std::swap(bufVelY, bufNewVelY);
    std::swap(bufVelZ, bufNewVelZ);

    return bodies;
}

OpenClSimulationSoA2SplitLoopTiled::~OpenClSimulationSoA2SplitLoopTiled() {
    if (bufPosX) clReleaseMemObject(bufPosX);
    if (bufPosY) clReleaseMemObject(bufPosY);
    if (bufPosZ) clReleaseMemObject(bufPosZ);
    if (bufMass) clReleaseMemObject(bufMass);
    if (bufGMass) clReleaseMemObject(bufGMass);

    if (bufVelX) clReleaseMemObject(bufVelX);
    if (bufVelY) clReleaseMemObject(bufVelY);
    if (bufVelZ) clReleaseMemObject(bufVelZ);

    if (bufNewPosX) clReleaseMemObject(bufNewPosX);
    if (bufNewPosY) clReleaseMemObject(bufNewPosY);
    if (bufNewPosZ) clReleaseMemObject(bufNewPosZ);

    if (bufNewVelX) clReleaseMemObject(bufNewVelX);
    if (bufNewVelY) clReleaseMemObject(bufNewVelY);
    if (bufNewVelZ) clReleaseMemObject(bufNewVelZ);

    if (kernelForces) clReleaseKernel(kernelForces);
    if (kernelGMass) clReleaseKernel(kernelGMass);
    if (program) clReleaseProgram(program);
    if (queue) clReleaseCommandQueue(queue);
    if (context) clReleaseContext(context);
}