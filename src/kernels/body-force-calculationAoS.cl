__kernel
void calculate_forces(
    __global const float4* pos,      // x, y, z, mass
    __global float4* vel,            // vx, vy, vz, unused
    __global float4* new_pos,        // output
    __global float4* new_vel,        // output
    const float timeStep,
    const float G,
    const float eps,
    const int numBodies)
{
    int i = get_global_id(0);
    if (i >= numBodies) return;

    float4 p_i = pos[i];
    float3 v_i = vel[i].xyz;
    float3 totalAcc = (float3)(0.0f, 0.0f, 0.0f);

    for (int j = 0; j < numBodies; j++) {
        if (i == j) continue;

        float4 p_j = pos[j];
        float3 direction = p_j.xyz - p_i.xyz;
        float distSq = dot(direction, direction) + eps;
        float invDistCube = rsqrt(distSq * distSq * distSq);

        totalAcc += G * p_j.w * direction * invDistCube;
    }

    v_i += totalAcc * timeStep;
    float3 nextPos = p_i.xyz + v_i * timeStep;

    new_pos[i] = (float4)(nextPos, p_i.w);
    new_vel[i] = (float4)(v_i, 0.0f);
}
