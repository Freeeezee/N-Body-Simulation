#ifndef N_BODY_SIMULATION_FILEUTIL_HPP
#define N_BODY_SIMULATION_FILEUTIL_HPP

#include <string>
#include <string_view>

bool saveToFile(std::string_view filename, std::string_view content);

std::string loadFromFile(std::string_view filename);

#endif //N_BODY_SIMULATION_FILEUTIL_HPP