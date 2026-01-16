#include "util/generateBodies.h"
#include "models/Body.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"

int main() {
    const auto bodies = generateBodies(
    1000,
    1.0f,
    1.0f,
    {-200.0f, -200.0f, -200.0f},
    {200.0f, 200.0f, 200.0f},
    {-1.0f, -1.0f, -1.0f},
    {1.0f, 1.0f, 1.0f},
    {
        Body(1.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}),
    });

    const auto serialized = serializeStringBodies(&bodies);

    saveToFile("generated_bodies.txt", serialized);

    return 0;
}
