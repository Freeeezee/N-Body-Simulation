#include "rendering/axes.h"

#include "glm/vec3.hpp"
#include "rendering/helpers.hpp"

std::pair<std::vector<GLfloat>, std::vector<GLfloat>> generateAxes() {
    std::vector<GLfloat> mesh;
    std::vector<GLfloat> colors;

    constexpr float len = 50.0f;
    constexpr float thk = 0.5f;

    // X-Axis (Red)
    addBox(mesh, colors, glm::vec3(len/2.0f, 0.0f, 0.0f), glm::vec3(len/2.0f, thk, thk), glm::vec3(1.0f, 0.0f, 0.0f));

    // Y-Axis (Green)
    addBox(mesh, colors, glm::vec3(0.0f, len/2.0f, 0.0f), glm::vec3(thk, len/2.0f, thk), glm::vec3(0.0f, 1.0f, 0.0f));

    // Z-Axis (Blue)
    addBox(mesh, colors, glm::vec3(0.0f, 0.0f, len/2.0f), glm::vec3(thk, thk, len/2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    return {mesh, colors};
}
