#include "utils.hpp"

json readJson(std::string fileName) {
    std::ifstream jsonFile(fileName);
    json brilProg = json::parse(jsonFile);
    return brilProg;
}

std::ofstream genOutFile(std::string inputName, std::string ext) {
    std::string outputJsonName = std::string(inputName);
    size_t dotPos = outputJsonName.rfind('.');
    if (dotPos != std::string::npos) {
        outputJsonName.insert(dotPos, ext);
    }
    std::ofstream outfile(outputJsonName);
    return outfile;
}