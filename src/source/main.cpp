#include <iostream>

#include "generateBodies.h"
#include "OpenGLFrameworkException.hpp"
#include "Simulation.hpp"
#include "rendering/Renderer.hpp"

int main()
{
    const auto bodies = generateBodies(
        500,
        1.0f,
        1.0f,
        {-200.0f, -200.0f, -200.0f},
        {200.0f, 200.0f, 200.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {
            Body(1000000.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}),
        });

    const auto simulation = new Simulation(1, bodies);

    try
    {
        const auto renderer = new Renderer();

        auto calculatedBodies = simulation->calculateNextTick();

        auto calculatedBodiesPtr = std::make_shared<decltype(calculatedBodies)>(calculatedBodies);
        renderer->setSpacebarHandler([simulation, calculatedBodiesPtr] {
            *calculatedBodiesPtr = simulation->calculateNextTick();
        });

        while( renderer->isWindowOpen() )
        {
            renderer->Draw(*calculatedBodiesPtr);
        }
    }
    catch( OpenGLFrameworkException& exception )
    {
        std::cerr << exception.what() << std::endl;
    }
}
