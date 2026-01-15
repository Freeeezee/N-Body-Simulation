#include "OpenGLFramework.hpp"
#include <iostream>
#include <vector>
#include <cmath> // Needed for sin/cos
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGLFrameworkException.hpp"
#include <GLFW/glfw3.h>

// Constants for Math
const float PI = 3.14159265359f;

// --- Helper: Add a single vertex with color to the buffers ---
void addVertex(std::vector<GLfloat>& vertices, std::vector<GLfloat>& colors,
               float x, float y, float z,
               float r, float g, float b) {
    vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
    colors.push_back(r);   colors.push_back(g);   colors.push_back(b);
}

// --- Helper: Generate a Cube/Box (used for Axes) ---
// Generates a box centered at offset with specified dimensions and color
void addBox(std::vector<GLfloat>& mesh, std::vector<GLfloat>& colData,
            glm::vec3 pos, glm::vec3 scale, glm::vec3 color) {

    // 8 corners of a cube, scaled and translated
    glm::vec3 v[8];
    for(int i=0; i<8; i++) {
        v[i].x = pos.x + ((i & 1) ? scale.x : -scale.x);
        v[i].y = pos.y + ((i & 2) ? scale.y : -scale.y);
        v[i].z = pos.z + ((i & 4) ? scale.z : -scale.z);
    }

    // Indices for 12 triangles (36 vertices) covering the cube
    int indices[] = {
        0,1,2, 1,3,2, // Front
        4,6,5, 5,6,7, // Back
        0,2,4, 4,2,6, // Left
        1,5,3, 3,5,7, // Right
        2,3,6, 6,3,7, // Top
        0,4,1, 1,4,5  // Bottom
    };

    for (int i = 0; i < 36; i++) {
        glm::vec3 p = v[indices[i]];
        addVertex(mesh, colData, p.x, p.y, p.z, color.r, color.g, color.b);
    }
}

// --- 1. Generate 3D Axes ---
// Returns pair: {Vertices, Colors}
std::pair<std::vector<GLfloat>, std::vector<GLfloat>> generateAxes() {
    std::vector<GLfloat> mesh;
    std::vector<GLfloat> colors;

    // Dimensions for the axis lines (length 50.0, thickness 0.5)
    float len = 50.0f;
    float thk = 0.5f;

    // X-Axis (Red) - Extends along X
    addBox(mesh, colors, glm::vec3(len/2.0f, 0.0f, 0.0f), glm::vec3(len/2.0f, thk, thk), glm::vec3(1.0f, 0.0f, 0.0f));

    // Y-Axis (Green) - Extends along Y
    addBox(mesh, colors, glm::vec3(0.0f, len/2.0f, 0.0f), glm::vec3(thk, len/2.0f, thk), glm::vec3(0.0f, 1.0f, 0.0f));

    // Z-Axis (Blue) - Extends along Z
    addBox(mesh, colors, glm::vec3(0.0f, 0.0f, len/2.0f), glm::vec3(thk, thk, len/2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    return {mesh, colors};
}

// --- 2. Generate Sphere Mesh ---
// Returns pair: {Vertices, Colors}
std::pair<std::vector<GLfloat>, std::vector<GLfloat>> generateSphere(float radius, int sectors, int stacks) {
    std::vector<GLfloat> mesh;
    std::vector<GLfloat> colors;

    // Generate vertices for a UV sphere
    // We generate triangles directly here
    for(int i = 0; i < stacks; ++i) {
        float phi1 = PI * float(i) / float(stacks);
        float phi2 = PI * float(i + 1) / float(stacks);

        for(int j = 0; j < sectors; ++j) {
            float theta1 = 2.0f * PI * float(j) / float(sectors);
            float theta2 = 2.0f * PI * float(j + 1) / float(sectors);

            // 4 vertices for a quad (two triangles)
            // We use spherical coordinates: x = r sin(phi) cos(theta) ...
            auto getPt = [&](float phi, float theta) -> glm::vec3 {
                return glm::vec3(
                    radius * sin(phi) * cos(theta),
                    radius * cos(phi),
                    radius * sin(phi) * sin(theta)
                );
            };

            glm::vec3 p1 = getPt(phi1, theta1);
            glm::vec3 p2 = getPt(phi1, theta2);
            glm::vec3 p3 = getPt(phi2, theta1);
            glm::vec3 p4 = getPt(phi2, theta2);

            // Triangle 1 (Top-Left, Top-Right, Bottom-Left)
            // Using a slight color gradient based on height (y) for visual depth
            glm::vec3 c = glm::vec3(0.0f, 0.5f + (p1.y/radius)*0.5f, 1.0f);

            // Add Triangle 1
            if(i != 0) { // Top cap doesn't need this triangle
                addVertex(mesh, colors, p1.x, p1.y, p1.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p2.x, p2.y, p2.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p3.x, p3.y, p3.z, c.r, c.g, c.b);
            }

            // Add Triangle 2
            if(i != stacks - 1) { // Bottom cap doesn't need this triangle
                addVertex(mesh, colors, p2.x, p2.y, p2.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p4.x, p4.y, p4.z, c.r, c.g, c.b);
                addVertex(mesh, colors, p3.x, p3.y, p3.z, c.r, c.g, c.b);
            }
        }
    }
    return {mesh, colors};
}

int main( int _argc, char ** _argv )
{
    try
    {
        OpenGLFramework opengl( "shader/vertex.glsl", "shader/fragment.glsl" );
        opengl.initialize();

        Window * window = opengl.createWindow( "3D Axes and Spheres", 1024, 768 );

        // Create VAOs
        VAO const axesVAO = window->createVAO();
        VAO const sphereVAO = window->createVAO();

        // 1. Generate and Upload Axes Data
        auto axesData = generateAxes();
        window->sendRenderData( axesVAO, axesData.first, axesData.second );

        // 2. Generate and Upload Sphere Data (A single sphere model)
        auto sphereData = generateSphere(15.0f, 20, 20); // Radius 15.0
        window->sendRenderData( sphereVAO, sphereData.first, sphereData.second );

        // 3. Define positions for our spheres in 3D space
        std::vector<glm::vec3> spherePositions = {
            glm::vec3(100.0f, 100.0f, 0.0f),
            glm::vec3(140.0f, 40.0f, 60.0f),
            glm::vec3(-100.0f, 0.0f, -100.0f),
            glm::vec3(0.0f, 120.0f, 0.0f)
        };

        clock_t lastInterval = clock();
        float rotationAngle = 0.0f;

        // Enable Depth Testing (Important for 3D overlap)
        glEnable(GL_DEPTH_TEST);

        while( window->isOpen() )
        {
            // Clear Color and Depth buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // --- Camera / View Control ---
            // Calculate rotation for the whole world view
            const clock_t now = clock();
            const float deltaTime = static_cast<float>(now - lastInterval) / CLOCKS_PER_SEC;
            lastInterval = now;
            rotationAngle += deltaTime * 30.0f; // Slower rotation

            // View Matrix: Move camera back 0.5 units, rotate around Y
            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -0.5f));
            //view = glm::rotate(view, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

            // Projection Matrix (Assuming your shader multiplies MVP)
            // Note: If you don't have a projection matrix, the 'view' acts as the ModelView

            // --- Render Axes ---
            // Axes are at (0,0,0) global, so we just pass the View matrix as the Model matrix
            window->renderTriangles( axesVAO, axesData.first.size() / 3, view );

            // --- Render Spheres ---
            // Reuse the single sphere mesh, but draw it multiple times at different locations
            for (const auto& pos : spherePositions) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                glm::mat4 mvp = view * model; // Combine View and Model transforms

                window->renderTriangles( sphereVAO, sphereData.first.size() / 3, mvp );
            }

            window->swapBuffer();
        }
    }
    catch( OpenGLFrameworkException const exception )
    {
        std::cerr << exception.what() << std::endl;
    }
}