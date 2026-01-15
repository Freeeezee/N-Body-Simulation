#ifndef N_BODY_SIMULATION_HELPERS_H
#define N_BODY_SIMULATION_HELPERS_H
#include <vector>

#include "GL/glew.h"
#include "glm/vec3.hpp"
#include "models/Body.hpp"

void addVertex(std::vector<GLfloat>& vertices, std::vector<GLfloat>& colors,
               float x, float y, float z,
               float r, float g, float b);

void addBox(std::vector<GLfloat>& mesh, std::vector<GLfloat>& colData,
            glm::vec3 pos, glm::vec3 scale, glm::vec3 color);

std::vector<glm::vec3> extractSpherePositions(const std::vector<Body>& bodies);

#endif //N_BODY_SIMULATION_HELPERS_H