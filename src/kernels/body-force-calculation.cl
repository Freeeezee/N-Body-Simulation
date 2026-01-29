#ifdef cl_khr_fp64
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

__kernel
void calculate_forces(
    __global const float* posX,
    __global const float* posY,
    __global const float* posZ,
    __global const float* mass,
    __global const float* velX,
    __global const float* velY,
    __global const float* velZ,
    __global float* newPosX,
    __global float* newPosY,
    __global float* newPosZ,
    __global float* newVelX,
    __global float* newVelY,
    __global float* newVelZ,
    const float timeStep,
    const float G,
    const float eps,
    const int numBodies)
{
    int i = get_global_id(0);
    if (i >= numBodies) return;

    float p_ix = posX[i];
    float p_iy = posY[i];
    float p_iz = posZ[i];
    float v_ix = velX[i];
    float v_iy = velY[i];
    float v_iz = velZ[i];

#ifdef cl_khr_fp64
    double accX = 0.0, accY = 0.0, accZ = 0.0;
    const double pxd = (double)p_ix, pyd = (double)p_iy, pzd = (double)p_iz;
    const double Gd = (double)G;
    const double epsd = (double)eps;

    for (int j = 0; j < numBodies; j++) {
        if (i == j) continue;

        double dx = (double)posX[j] - pxd;
        double dy = (double)posY[j] - pyd;
        double dz = (double)posZ[j] - pzd;

        double distSq = dx*dx + dy*dy + dz*dz + epsd;
        double invDistCubed = 1.0 / (distSq * sqrt(distSq));
        double s = Gd * (double)mass[j] * invDistCubed;

        accX += s * dx;
        accY += s * dy;
        accZ += s * dz;
    }

    v_ix += (float)(accX * (double)timeStep);
    v_iy += (float)(accY * (double)timeStep);
    v_iz += (float)(accZ * (double)timeStep);
#else
    float accX = 0.0f, accY = 0.0f, accZ = 0.0f;

    for (int j = 0; j < numBodies; j++) {
        if (i == j) continue;

        float dx = posX[j] - p_ix;
        float dy = posY[j] - p_iy;
        float dz = posZ[j] - p_iz;

        float distSq = dx*dx + dy*dy + dz*dz + eps;
        float invDistCube = 1.0f / (distSq * sqrt(distSq));
        float s = G * mass[j] * invDistCube;

        accX += s * dx;
        accY += s * dy;
        accZ += s * dz;
    }

    v_ix += accX * timeStep;
    v_iy += accY * timeStep;
    v_iz += accZ * timeStep;
#endif

    newPosX[i] = p_ix + v_ix * timeStep;
    newPosY[i] = p_iy + v_iy * timeStep;
    newPosZ[i] = p_iz + v_iz * timeStep;
    newVelX[i] = v_ix;
    newVelY[i] = v_iy;
    newVelZ[i] = v_iz;
}