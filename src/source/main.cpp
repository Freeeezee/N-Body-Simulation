#include <iostream>
#include "OpenGLFrameworkException.hpp"
#include "Simulation.hpp"
#include "rendering/Renderer.hpp"

int main()
{
    const auto bodies = std::vector{
        Body( 1, glm::vec3 { 0, 0, 1 }, glm::vec3 { 0, 0, 0 } ),
    };

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
