#ifndef N_BODY_SIMULATION_SPHERE_H
#define N_BODY_SIMULATION_SPHERE_H
#include <utility>
#include <vector>

#include "GL/glew.h"

std::pair<std::vector<GLfloat>, std::vector<GLfloat>> generateSphere(float radius, int sectors, int stacks);

#endif //N_BODY_SIMULATION_SPHERE_H