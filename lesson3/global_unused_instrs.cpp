#include <jsoncpp/json/json.h>

#include <fstream>
#include <iostream>
#include <set>

bool programChanged = false;

bool removeNullValues(Json::Value&);

int main(int argc, char* argv[]) {
    std::string outputJsonName = std::string(argv[1]);
    size_t dotPos = outputJsonName.rfind('.');
    if (dotPos != std::string::npos) {
        outputJsonName.insert(dotPos, "_dce1");
    }
    std::ofstream outfile(outputJsonName);

    // Iterate over all instructions globally and delete the ones that
    // 1. are not used anywhere globally; 2. have no side effects
    std::ifstream jsonFile(argv[1]);
    Json::Value brilProg;
    jsonFile >> brilProg;

    do {
        // First iteration through the whole program to store all used variables
        std::set<std::string> usedVars;
        for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
            const Json::Value brilInstrucions =
                brilProg["functions"][fcnIdx]["instrs"];
            for (int instrIdx = 0; instrIdx < brilInstrucions.size();
                 ++instrIdx) {
                if (brilInstrucions[instrIdx].isMember("args")) {
                    for (auto arg : brilInstrucions[instrIdx]["args"]) {
                        usedVars.insert(arg.asString());
                    }
                }
            }
        }

        // Second iteration through the whole program to eliminate the
        // instruction that has dead variable(s) and has no side effects
        bool changedFlag = false;
        for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
            Json::Value& brilInstrucions =
                brilProg["functions"][fcnIdx]["instrs"];
            for (int instrIdx = 0; instrIdx < brilInstrucions.size();
                 ++instrIdx) {
                if (brilInstrucions[instrIdx].isMember("dest") &&
                    (usedVars.find(
                         brilInstrucions[instrIdx]["dest"].asString()) ==
                     usedVars
                         .end())) {  // bril has value operations vs. effect
                                     // operations; value operations have "dest"
                                     // fields and never have side effects, and
                                     // vice versa for effect operations
                    brilInstrucions[instrIdx].clear();
                    changedFlag = true;
                }
            }
        }
        programChanged = changedFlag;
        removeNullValues(brilProg);
    } while (programChanged);

    Json::FastWriter styledWriter;
    outfile << brilProg;
    outfile.close();

    return EXIT_SUCCESS;
}

bool removeNullValues(Json::Value& value) {
    if (value.isObject()) {
        std::vector<std::string> keysToRemove;
        for (auto it = value.begin(); it != value.end(); ++it) {
            if (it->isNull() || (it->isObject() && it->empty())) {
                keysToRemove.push_back(it.key().asString());
            } else {
                if (removeNullValues(*it)) {
                    keysToRemove.push_back(it.key().asString());
                }
            }
        }
        for (const auto& key : keysToRemove) {
            value.removeMember(key);
        }
        return value.empty();
    } else if (value.isArray()) {
        Json::ArrayIndex index = 0;
        while (index < value.size()) {
            if (value[index].isNull() ||
                (value[index].isObject() && value[index].empty())) {
                Json::Value removedItem;
                value.removeIndex(index, &removedItem);
            } else if (removeNullValues(value[index])) {
                Json::Value removedItem;
                value.removeIndex(index, &removedItem);
            } else {
                index++;
            }
        }
    }
    return value.isNull() || (value.isObject() && value.empty());
}
