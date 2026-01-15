//
// Created by Tom on 15.01.26.
//

#ifndef N_BODY_SIMULATION_RENDERER_H
#define N_BODY_SIMULATION_RENDERER_H
#include <vector>

#include "Window.hpp"
#include "models/Body.hpp"


class Renderer {
public:
    Renderer();

    void Draw(const std::vector<Body> &bodies) const;

    [[nodiscard]] bool isWindowOpen() const { return window->isOpen(); }
    void setSpacebarHandler(std::function<void()> _handler) const;

private:
    Window * window;
    VAO axesVAO;
    VAO sphereVAO;
    std::pair<std::vector<GLfloat>, std::vector<GLfloat>> axesData;
    std::pair<std::vector<GLfloat>, std::vector<GLfloat>> sphereData;
};


#endif //N_BODY_SIMULATION_RENDERER_H