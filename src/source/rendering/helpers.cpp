#include "rendering/helpers.hpp"

void addVertex(std::vector<GLfloat>& vertices, std::vector<GLfloat>& colors,
               const float x, const float y, const float z,
               const float r, const float g, const float b) {
    vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
    colors.push_back(r);   colors.push_back(g);   colors.push_back(b);
}

void addBox(std::vector<GLfloat>& mesh, std::vector<GLfloat>& colData,
            const glm::vec3 pos, const glm::vec3 scale, const glm::vec3 color) {

    glm::vec3 v[8];
    for(int i=0; i<8; i++) {
        v[i].x = pos.x + ((i & 1) ? scale.x : -scale.x);
        v[i].y = pos.y + ((i & 2) ? scale.y : -scale.y);
        v[i].z = pos.z + ((i & 4) ? scale.z : -scale.z);
    }

    for (int i = 0; i < 36; i++) {
        const int indices[] = {
            0,1,2, 1,3,2, // Front
            4,6,5, 5,6,7, // Back
            0,2,4, 4,2,6, // Left
            1,5,3, 3,5,7, // Right
            2,3,6, 6,3,7, // Top
            0,4,1, 1,4,5  // Bottom
        };

        const glm::vec3 p = v[indices[i]];

        addVertex(mesh, colData, p.x, p.y, p.z, color.r, color.g, color.b);
    }
}

std::vector<glm::vec3> extractSpherePositions(const std::vector<Body> &bodies) {
    std::vector<glm::vec3> positions;
    for (const auto& body : bodies) {
        positions.push_back(body.position);
    }
    return positions;
}

