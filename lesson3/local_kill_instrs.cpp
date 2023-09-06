#include <any>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <typeinfo>
#include <vector>

using json = nlohmann::json;

using Var = std::string;
using Instr = json;
using Block = std::vector<Instr*>;

bool programChanged = false;

std::set<std::string> TERMINATOR_OPS = {"jmp", "br", "ret"};

std::vector<Block> formBasicBlocks(const std::vector<Instr*>&);
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
        std::vector<Block> allBlocks;
        for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
            std::vector<Instr*> brilInstrs;
            for (auto& instr : brilProg["functions"][fcnIdx]["instrs"]) {
                brilInstrs.push_back(&instr);
            }
            std::vector<Block> blocks = formBasicBlocks(brilInstrs);
            for (const auto& block : blocks) {
                allBlocks.push_back(block);
            }
        }

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

bool isTerminator(const Instr& instr) {
    return (instr.contains("labels") &&
            (TERMINATOR_OPS.find(instr["op"]) != TERMINATOR_OPS.end()));
}

std::vector<Block> formBasicBlocks(const std::vector<Instr*>& instrs) {
    std::vector<Block> basicBlocks;
    Block curBlock;
    for (const auto& instr : instrs) {
        if (isTerminator(*instr)) {
            curBlock.push_back(instr);
            basicBlocks.push_back(curBlock);
            curBlock.clear();
        } else if ((*instr).contains("label")) {
            if (curBlock.size() > 0) {
                basicBlocks.push_back(curBlock);
            }
            curBlock = {instr};
        } else {
            curBlock.push_back(instr);
        }
    }
    if (curBlock.size() > 0) {
        basicBlocks.push_back(curBlock);
    }

    return basicBlocks;
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