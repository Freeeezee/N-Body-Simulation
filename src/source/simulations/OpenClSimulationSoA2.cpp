#include "simulations/OpenClSimulationSoA2.hpp"
#include "Constants.hpp"
#include <fstream>
#include <vector>
#include <iostream>

OpenClSimulationSoA2::OpenClSimulationSoA2(const float timeStep, const std::vector<Body> &bodies)
    : SimulationSoA2(timeStep, bodies) {
    initOpenCL();
    updateBuffers();
}

void OpenClSimulationSoA2::initOpenCL() {
    cl_uint num_platforms;
    clGetPlatformIDs(1, nullptr, &num_platforms);
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &deviceId, nullptr);
    context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, nullptr);
    queue = clCreateCommandQueue(context, deviceId, 0, nullptr);

    std::ifstream kernelFile("kernels/body-force-calculationSoA.cl");
    const std::string src((std::istreambuf_iterator(kernelFile)), std::istreambuf_iterator<char>());
    const char* srcPtr = src.c_str();
    const size_t srcSize = src.length();

    program = clCreateProgramWithSource(context, 1, &srcPtr, &srcSize, nullptr);

    const auto buildOptions =
        "-cl-std=CL1.2 "
        "-cl-fp32-correctly-rounded-divide-sqrt";

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

    const size_t n = bodies.masses.size();
    const size_t floatSize = n * sizeof(float);
    
    bufPosX = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufPosY = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufPosZ = clCreateBuffer(context, CL_MEM_READ_WRITE, floatSize, nullptr, nullptr);
    bufMass = clCreateBuffer(context, CL_MEM_READ_ONLY, floatSize, nullptr, nullptr);
    
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
}

void OpenClSimulationSoA2::updateBuffers() {
    const size_t n = bodies.masses.size();
    const size_t floatSize = n * sizeof(float);

    clEnqueueWriteBuffer(queue, bufPosX, CL_TRUE, 0, floatSize, bodies.posX.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufPosY, CL_TRUE, 0, floatSize, bodies.posY.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufPosZ, CL_TRUE, 0, floatSize, bodies.posZ.data(), 0, nullptr, nullptr);

    clEnqueueWriteBuffer(queue, bufVelX, CL_TRUE, 0, floatSize, bodies.velX.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufVelY, CL_TRUE, 0, floatSize, bodies.velY.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(queue, bufVelZ, CL_TRUE, 0, floatSize, bodies.velZ.data(), 0, nullptr, nullptr);
}

BodiesSoA2 OpenClSimulationSoA2::calculateNextTick() {
    const int numBodies = static_cast<int>(bodies.masses.size());
    
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufPosX);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufPosY);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufPosZ);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufMass);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), &bufVelX);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), &bufVelY);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), &bufVelZ);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), &bufNewPosX);
    clSetKernelArg(kernel, 8, sizeof(cl_mem), &bufNewPosY);
    clSetKernelArg(kernel, 9, sizeof(cl_mem), &bufNewPosZ);
    clSetKernelArg(kernel, 10, sizeof(cl_mem), &bufNewVelX);
    clSetKernelArg(kernel, 11, sizeof(cl_mem), &bufNewVelY);
    clSetKernelArg(kernel, 12, sizeof(cl_mem), &bufNewVelZ);
    
    clSetKernelArg(kernel, 13, sizeof(float), &timeStep);
    constexpr float g_val = G;
    constexpr float eps_val = DISTANCE_EPSILON;
    clSetKernelArg(kernel, 14, sizeof(float), &g_val);
    clSetKernelArg(kernel, 15, sizeof(float), &eps_val);
    clSetKernelArg(kernel, 16, sizeof(int), &numBodies);

    size_t globalSize = numBodies;
    clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);

    const size_t floatSize = static_cast<size_t>(numBodies) * sizeof(float);

    clEnqueueReadBuffer(queue, bufNewPosX, CL_TRUE, 0, floatSize, bodies.posX.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewPosY, CL_TRUE, 0, floatSize, bodies.posY.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewPosZ, CL_TRUE, 0, floatSize, bodies.posZ.data(), 0, nullptr, nullptr);

    clEnqueueReadBuffer(queue, bufNewVelX, CL_TRUE, 0, floatSize, bodies.velX.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewVelY, CL_TRUE, 0, floatSize, bodies.velY.data(), 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, bufNewVelZ, CL_TRUE, 0, floatSize, bodies.velZ.data(), 0, nullptr, nullptr);

    std::swap(bufPosX, bufNewPosX); std::swap(bufPosY, bufNewPosY); std::swap(bufPosZ, bufNewPosZ);
    std::swap(bufVelX, bufNewVelX); std::swap(bufVelY, bufNewVelY); std::swap(bufVelZ, bufNewVelZ);

    return bodies;
}

OpenClSimulationSoA2::~OpenClSimulationSoA2() {
    clReleaseMemObject(bufPosX); clReleaseMemObject(bufPosY); clReleaseMemObject(bufPosZ);
    clReleaseMemObject(bufMass);
    clReleaseMemObject(bufVelX); clReleaseMemObject(bufVelY); clReleaseMemObject(bufVelZ);
    clReleaseMemObject(bufNewPosX); clReleaseMemObject(bufNewPosY); clReleaseMemObject(bufNewPosZ);
    clReleaseMemObject(bufNewVelX); clReleaseMemObject(bufNewVelY); clReleaseMemObject(bufNewVelZ);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
