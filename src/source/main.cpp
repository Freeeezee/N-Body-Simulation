#include <iostream>

#include "generateBodies.h"
#include "OpenGLFrameworkException.hpp"
#include "Simulation.hpp"
#include "rendering/Renderer.hpp"

int main()
{
    const auto bodies = generateBodies(
        500,
        {-200.0f, -200.0f, -200.0f},
        {200.0f, 200.0f, 200.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f});

    const auto simulation = new Simulation(1, bodies);

    try
    {
        const auto renderer = new Renderer();

        while( renderer->isWindowOpen() )
        {
            renderer->Draw(simulation->calculateNextTick());
        }
    }
    catch( OpenGLFrameworkException& exception )
    {
        std::cerr << exception.what() << std::endl;
    }
}
