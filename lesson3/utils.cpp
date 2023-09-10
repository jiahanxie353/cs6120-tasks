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

bool removeNullValues(json& value) {
    if (value.is_object()) {
        std::vector<std::string> keysToRemove;

        for (auto it = value.begin(); it != value.end(); ++it) {
            if (it->is_null() || (it->is_object() && it->empty())) {
                keysToRemove.push_back(it.key());
            } else {
                if (removeNullValues(*it)) {
                    keysToRemove.push_back(it.key());
                }
            }
        }
        for (const auto& key : keysToRemove) {
            value.erase(key);
        }
        return value.empty();
    } else if (value.is_array()) {
        for (auto it = value.begin(); it != value.end();
             /* no increment here */) {
            if (it->is_null() || (it->is_object() && it->empty())) {
                it = value.erase(it);
            } else if (removeNullValues(*it)) {
                it = value.erase(it);
            } else {
                ++it;
            }
        }
    }
    return value.is_null() || (value.is_object() && value.empty());
}