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

    float accX = 0.0f, accY = 0.0f, accZ = 0.0f;

    for (int j = 0; j < numBodies; j++) {
        if (i == j) continue;

        float dx = posX[j] - p_ix;
        float dy = posY[j] - p_iy;
        float dz = posZ[j] - p_iz;

        float distSq = dx*dx + dy*dy + dz*dz + eps;
        float invDistCube = rsqrt(distSq * distSq * distSq);
        float s = G * mass[j] * invDistCube;

        accX += s * dx;
        accY += s * dy;
        accZ += s * dz;
    }

    v_ix += accX * timeStep;
    v_iy += accY * timeStep;
    v_iz += accZ * timeStep;

    newPosX[i] = p_ix + v_ix * timeStep;
    newPosY[i] = p_iy + v_iy * timeStep;
    newPosZ[i] = p_iz + v_iz * timeStep;
    newVelX[i] = v_ix;
    newVelY[i] = v_iy;
    newVelZ[i] = v_iz;
}