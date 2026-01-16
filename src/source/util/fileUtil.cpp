#include "util/fileUtil.hpp"
#include <fstream>
#include <sstream>

bool saveToFile(std::string_view filename, std::string_view content) {
    std::ofstream outFile((std::string(filename)));
    if (!outFile) {
        return false;
    }
    outFile << content;
    return true;
}

std::string loadFromFile(std::string_view filename) {
    std::ifstream inFile((std::string(filename)));
    if (!inFile) {
        return "";
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
}