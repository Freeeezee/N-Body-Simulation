#include "simulations/OpenClSimulationAoS.hpp"
#include "Constants.hpp"
#include <fstream>
#include <vector>

OpenClSimulationAoS::OpenClSimulationAoS(const float timeStep, const std::vector<Body> &bodies)
    : Simulation(timeStep, bodies) {
    initOpenCL();
    updateBuffers();
}

void OpenClSimulationAoS::initOpenCL() {
    cl_uint num_platforms;
    clGetPlatformIDs(1, NULL, &num_platforms);
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device_id, 0, NULL);

    std::ifstream kernelFile("kernels/body-force-calculationAoS.cl");
    std::string src((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    const char* src_ptr = src.c_str();
    size_t src_size = src.length();

    program = clCreateProgramWithSource(context, 1, &src_ptr, &src_size, NULL);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "calculate_forces", NULL);

    size_t size = bodies.size() * sizeof(cl_float4);
    bufPosMass = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, NULL);
    bufVel = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, NULL);
    bufNewPos = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, NULL);
    bufNewVel = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, NULL);
}

void OpenClSimulationAoS::updateBuffers() {
    std::vector<cl_float4> posMass(bodies.size());
    std::vector<cl_float4> vel(bodies.size());

    for (size_t i = 0; i < bodies.size(); ++i) {
        posMass[i] = {bodies[i].position.x, bodies[i].position.y, bodies[i].position.z, bodies[i].mass};
        vel[i] = {bodies[i].velocity.x, bodies[i].velocity.y, bodies[i].velocity.z, 0.0f};
    }

    clEnqueueWriteBuffer(queue, bufPosMass, CL_TRUE, 0, posMass.size() * sizeof(cl_float4), posMass.data(), 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufVel, CL_TRUE, 0, vel.size() * sizeof(cl_float4), vel.data(), 0, NULL, NULL);
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
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    std::vector<cl_float4> resPos(numBodies);
    std::vector<cl_float4> resVel(numBodies);
    clEnqueueReadBuffer(queue, bufNewPos, CL_TRUE, 0, numBodies * sizeof(cl_float4), resPos.data(), 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufNewVel, CL_TRUE, 0, numBodies * sizeof(cl_float4), resVel.data(), 0, NULL, NULL);

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
