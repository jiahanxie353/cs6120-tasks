#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <set>

#include "utils.hpp"

using json = nlohmann::json;

bool programChanged = false;

int main(int argc, char* argv[]) {
    std::string outputJsonName = std::string(argv[1]);
    size_t dotPos = outputJsonName.rfind('.');
    if (dotPos != std::string::npos) {
        outputJsonName.insert(dotPos, "_dce1");
    }
    std::ofstream outfile(outputJsonName);

    std::ifstream jsonFile(argv[1]);
    json brilProg = json::parse(jsonFile);

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

    outfile << brilProg.dump(4);
    outfile.close();

    return EXIT_SUCCESS;
}
