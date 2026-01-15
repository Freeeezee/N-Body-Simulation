#include "rendering/Renderer.hpp"

#include "OpenGLFramework.hpp"
#include "GL/glew.h"
#include "rendering/axes.h"
#include "rendering/sphere.h"
#include <glm/gtc/matrix_transform.hpp>

#include "rendering/helpers.hpp"

Renderer::Renderer() {
    OpenGLFramework opengl( "shader/vertex.glsl", "shader/fragment.glsl" );
    opengl.initialize();

    window = opengl.createWindow( "N-Body Simulation", 1024, 768 );

    axesVAO = window->createVAO();
    sphereVAO = window->createVAO();

    axesData = generateAxes();
    window->sendRenderData( axesVAO, axesData.first, axesData.second );

    sphereData = generateSphere(1.0f, 5, 5);
    window->sendRenderData( sphereVAO, sphereData.first, sphereData.second );

    std::vector spherePositions = {
        glm::vec3(100.0f, 100.0f, 0.0f),
        glm::vec3(140.0f, 40.0f, 60.0f),
        glm::vec3(-100.0f, 0.0f, -100.0f),
        glm::vec3(0.0f, 120.0f, 0.0f)
    };

    glEnable(GL_DEPTH_TEST);
}

void Renderer::Draw(const std::vector<Body> &bodies) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    constexpr glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -0.5f));

    window->renderTriangles( axesVAO, axesData.first.size() / 3, view );

    const auto spherePositions = extractSpherePositions(bodies);

    for (const auto& pos : spherePositions) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 mvp = view * model;

        window->renderTriangles( sphereVAO, sphereData.first.size() / 3, mvp );
    }

    window->swapBuffer();
}
