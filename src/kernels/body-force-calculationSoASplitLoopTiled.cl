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
    const int numBodies,
    __local float* lPosX,
    __local float* lPosY,
    __local float* lPosZ,
    __local float* lGMass)
{
    const int i = get_global_id(0);
    if (i >= numBodies) return;

    const int lid = get_local_id(0);
    const int lsz = (int)get_local_size(0);

    const float p_ix = posX[i];
    const float p_iy = posY[i];
    const float p_iz = posZ[i];

    float v_ix = velX[i];
    float v_iy = velY[i];
    float v_iz = velZ[i];

    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;

    for (int tileBase = 0; tileBase < numBodies; tileBase += lsz) {
        const int j = tileBase + lid;

        // Cooperative load into local memory (guarded)
        if (j < numBodies) {
            lPosX[lid] = posX[j];
            lPosY[lid] = posY[j];
            lPosZ[lid] = posZ[j];
            lGMass[lid] = gMass[j];
        } else {
            lPosX[lid] = 0.0f;
            lPosY[lid] = 0.0f;
            lPosZ[lid] = 0.0f;
            lGMass[lid] = 0.0f;
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        const int tileCount = min(lsz, numBodies - tileBase);

        for (int tj = 0; tj < tileCount; ++tj) {
            const int gj = tileBase + tj;
            if (gj == i) continue;

            const float dx = lPosX[tj] - p_ix;
            const float dy = lPosY[tj] - p_iy;
            const float dz = lPosZ[tj] - p_iz;

            const float distSq = dx*dx + dy*dy + dz*dz + eps;

            const float invDist = rsqrt(distSq);
            const float invDistCube = invDist * invDist * invDist;

            const float s = lGMass[tj] * invDistCube;

            accX += s * dx;
            accY += s * dy;
            accZ += s * dz;
        }

        barrier(CLK_LOCAL_MEM_FENCE);
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