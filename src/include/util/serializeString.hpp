#ifndef N_BODY_SIMULATION_SERIALIZESTRING_HPP
#define N_BODY_SIMULATION_SERIALIZESTRING_HPP
#include <string>
#include <vector>
#include "../models/Body.hpp"

std::string serializeStringBodies(const std::vector<Body> *bodies);
std::vector<Body> deserializeStringBodies(const std::string& serializedBodies);

#endif //N_BODY_SIMULATION_SERIALIZESTRING_HPP
