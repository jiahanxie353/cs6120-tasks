#include <any>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <typeinfo>
#include <vector>

#include "cfg.hpp"

bool programChanged = false;

bool removeNullValues(json&);

int main(int argc, char* argv[]) {
    std::string outputJsonName = std::string(argv[1]);
    size_t dotPos = outputJsonName.rfind('.');
    if (dotPos != std::string::npos) {
        outputJsonName.insert(dotPos, "_dce2");
    }
    std::ofstream outfile(outputJsonName);

    std::ifstream jsonFile(argv[1]);
    json brilProg = json::parse(jsonFile);

    do {
        std::vector<Block> allBlocks = genAllBlocks(brilProg);

        std::map<Var, Instr*> lastDef;  // keeps track of variables that are
                                        // defined but never used
        bool changedFlag = false;
        for (auto& block : allBlocks) {
            for (auto& instr : block) {
                // check for uses first
                if ((*instr).contains("args")) {
                    for (const auto& arg : (*instr)["args"]) {
                        lastDef.erase(arg.dump());
                    }
                }
                // then check for defs
                if ((*instr).contains("dest")) {
                    Var candidate = (*instr)["dest"];
                    if (lastDef.find(candidate) != lastDef.end()) {
                        (*lastDef[candidate]).clear();
                        changedFlag = true;
                    }
                    lastDef.insert({candidate, instr});
                }
            }
        }
        programChanged = changedFlag;
        removeNullValues(brilProg);
    } while (programChanged);

    outfile << brilProg.dump(4);
    outfile.close();

    return EXIT_SUCCESS;
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