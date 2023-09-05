#include <any>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <typeinfo>
#include <vector>

using json = nlohmann::json;

using Instr = json;
using Block = std::vector<Instr>;

std::set<std::string> TERMINATOR_OPS = {"jmp", "br", "ret"};

std::vector<Block> formBasicBlocks(const std::vector<Instr>&);

int main(int argc, char* argv[]) {
    std::ifstream jsonFile(argv[1]);
    json brilProg = json::parse(jsonFile);

    std::vector<Block> allBlocks;
    for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
        const json brilInstrucions = brilProg["functions"][fcnIdx]["instrs"];
        std::vector<Block> blocks = formBasicBlocks(brilInstrucions);
        for (const auto& block : blocks) {
            allBlocks.push_back(block);
        }
    }
    for (const auto& block : allBlocks) {
        std::cout << "[" << std::endl;
        for (const auto& instr : block) {
            std::cout << instr.dump() << std::endl;
        }
        std::cout << "]" << std::endl;
    }

    return EXIT_SUCCESS;
}

bool isTerminator(const Instr& instr) {
    return (instr.contains("labels") &&
            (TERMINATOR_OPS.find(instr["op"]) != TERMINATOR_OPS.end()));
}

std::vector<Block> formBasicBlocks(const std::vector<Instr>& instrs) {
    std::vector<Block> basicBlocks;
    Block curBlock;
    for (const auto& instr : instrs) {
        if (isTerminator(instr)) {
            curBlock.push_back(instr);
            basicBlocks.push_back(curBlock);
            curBlock.clear();
        } else if (instr.contains("label")) {
            if (curBlock.size() > 0) {
                basicBlocks.push_back(curBlock);
            }
            curBlock = {instr};
        } else {
            curBlock.push_back(instr);
        }
    }
    basicBlocks.push_back(curBlock);

    return basicBlocks;
}