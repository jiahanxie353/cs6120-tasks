#include "local_kill_instrs.hpp"

json local_kill(json brilProg) {
    bool programChanged = false;
    do {
        std::vector<Block> allBlocks = genAllBlocks(brilProg);

        std::map<Var, Instr*> lastDef;  // keeps track of variables that are
                                        // defined but never used
        bool changedFlag = false;
        for (auto& block : allBlocks) {
            for (auto& instr : block) {
                // check for uses first
                if (instr->contains("args")) {
                    for (const auto& arg : (*instr)["args"]) {
                        lastDef.erase(arg.get<std::string>());
                    }
                }
                // then check for defs
                if (instr->contains("dest")) {
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

    return brilProg;
}

// int main(int arg, char* argv[]) {
//     json brilProg = readJson(argv[1]);
//     local_kill(brilProg);
// }
