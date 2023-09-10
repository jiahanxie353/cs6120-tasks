#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <set>

#include "utils.hpp"

using json = nlohmann::json;

json global_unused(json brilProg) {
    bool programChanged = false;
    do {
        // First iteration through the whole program to store all used variables
        std::set<std::string> usedVars;
        for (auto& function : brilProg["functions"]) {
            for (const auto& instr : function["instrs"]) {
                if (instr.contains("args")) {
                    for (const auto& arg : instr["args"]) {
                        usedVars.insert(arg.get<std::string>());
                    }
                }
            }
        }

        // Second iteration through the whole program to eliminate the
        // instruction that has dead variable(s) and has no side effects
        bool changedFlag = false;
        for (auto& function : brilProg["functions"]) {
            for (auto& instr : function["instrs"]) {
                if (instr.contains("dest") &&
                    (usedVars.find(instr["dest"].get<std::string>()) ==
                     usedVars.end())) {  // bril has value operations vs. effect
                    // operations; value operations have "dest"
                    // fields and never have side effects, and
                    // vice versa for effect operations
                    instr.clear();
                    changedFlag = true;
                }
            }
        }
        programChanged = changedFlag;
        removeNullValues(brilProg);
    } while (programChanged);

    return brilProg;
}
