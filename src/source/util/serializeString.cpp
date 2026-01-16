#include "util/serializeString.hpp"

#include <sstream>

std::string serializeStringBodies(const std::vector<Body> *bodies) {
    if (!bodies) return "";

    std::stringstream ss;
    for (const auto& body : *bodies) {
        ss << body.mass << " "
           << body.position.x << " " << body.position.y << " " << body.position.z << " "
           << body.velocity.x << " " << body.velocity.y << " " << body.velocity.z << "\n";
    }
    return ss.str();
}

std::vector<Body> deserializeStringBodies(const std::string& serializedBodies) {
    std::vector<Body> bodies;
    std::stringstream ss(serializedBodies);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty()) continue;

        std::stringstream lineStream(line);
        Body body;
        if (lineStream >> body.mass >>
            body.position.x >> body.position.y >> body.position.z >>
            body.velocity.x >> body.velocity.y >> body.velocity.z) {
            bodies.push_back(body);
            }
    }

    return bodies;
}
