#include "util/bodies.h"
#include "models/Body.hpp"
#include "util/fileUtil.hpp"
#include "util/serializeString.hpp"

int main() {
    const auto bodies = generateBodies(
    20000,
    1.0f,
    1.0f,
    {-200.0f, -200.0f, -200.0f},
    {200.0f, 200.0f, 200.0f},
    {-1.0f, -1.0f, -1.0f},
    {1.0f, 1.0f, 1.0f}
    );

    const auto serialized = serializeStringBodies(&bodies);

    saveToFile("generated_bodies.txt", serialized);

    return 0;
}
