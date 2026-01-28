#include "util/fileUtil.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

bool saveToFile(const std::string_view filename, const std::string_view content) {
    std::ofstream outFile((std::string(filename)));
    if (!outFile) {
        return false;
    }
    outFile << content;
    return true;
}

std::string loadFromFile(const std::string_view filename) {
    const std::ifstream inFile((std::string(filename)));
    if (!inFile) {
        return "";
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
}

bool fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}