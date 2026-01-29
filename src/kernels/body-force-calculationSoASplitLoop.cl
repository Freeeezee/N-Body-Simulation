__kernel
void compute_gmasses(
    __global const float* mass,
    __global float* gMass,
    const float G,
    const int numBodies)
{
    const int i = get_global_id(0);
    if (i >= numBodies) return;
    gMass[i] = G * mass[i];
}

__kernel
void calculate_forces_split(
    __global const float* posX,
    __global const float* posY,
    __global const float* posZ,
    __global const float* gMass,
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
    const float eps,
    const int numBodies)
{
    const int i = get_global_id(0);
    if (i >= numBodies) return;

    const float p_ix = posX[i];
    const float p_iy = posY[i];
    const float p_iz = posZ[i];

    float v_ix = velX[i];
    float v_iy = velY[i];
    float v_iz = velZ[i];

    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;

    for (int j = 0; j < i; ++j) {
        const float dx = posX[j] - p_ix;
        const float dy = posY[j] - p_iy;
        const float dz = posZ[j] - p_iz;

        const float distSq = dx*dx + dy*dy + dz*dz + eps;
        const float invDist = 1.0f / sqrt(distSq);
        const float invDistCube = invDist * invDist * invDist;

        const float s = gMass[j] * invDistCube;

        accX += s * dx;
        accY += s * dy;
        accZ += s * dz;
    }

    for (int j = i + 1; j < numBodies; ++j) {
        const float dx = posX[j] - p_ix;
        const float dy = posY[j] - p_iy;
        const float dz = posZ[j] - p_iz;

        const float distSq = dx*dx + dy*dy + dz*dz + eps;
        const float invDist = 1.0f / sqrt(distSq);
        const float invDistCube = invDist * invDist * invDist;

        const float s = gMass[j] * invDistCube;

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