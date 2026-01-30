#include "rendering/sphere.h"

#include "Constants.hpp"
#include "glm/vec3.hpp"
#include "rendering/helpers.hpp"
#include <cmath>

std::pair<std::vector<GLfloat>, std::vector<GLfloat>> generateSphere(const float radius, const int sectors, const int stacks) {
    std::vector<GLfloat> mesh;
    std::vector<GLfloat> colors;

    for(int i = 0; i < stacks; ++i) {
        const float phi1 = PI * static_cast<float>(i) / static_cast<float>(stacks);
        const float phi2 = PI * static_cast<float>(i + 1) / static_cast<float>(stacks);

        for(int j = 0; j < sectors; ++j) {
            const float theta1 = 2.0f * PI * static_cast<float>(j) / static_cast<float>(sectors);
            const float theta2 = 2.0f * PI * static_cast<float>(j + 1) / static_cast<float>(sectors);

            auto getPt = [&](float phi, float theta) -> glm::vec3 {
                return glm::vec3(
                    radius * std::sin(phi) * std::cos(theta),
                    radius * std::cos(phi),
                    radius * std::sin(phi) * std::sin(theta)
                );
            };

            const glm::vec3 p1 = getPt(phi1, theta1);
            const glm::vec3 p2 = getPt(phi1, theta2);
            const glm::vec3 p3 = getPt(phi2, theta1);
            const glm::vec3 p4 = getPt(phi2, theta2);

            const auto c = glm::vec3(0.0f, 0.5f + (p1.y/radius)*0.5f, 1.0f);

            if(i != 0) {
                addVertex(mesh, colors, p1.x, p1.y, p1.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p2.x, p2.y, p2.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p3.x, p3.y, p3.z, c.r, c.g, c.b);
            }

            if(i != stacks - 1) {
                addVertex(mesh, colors, p2.x, p2.y, p2.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p4.x, p4.y, p4.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p3.x, p3.y, p3.z, c.r, c.g, c.b);
            }
        }
    }
    return {mesh, colors};
}
