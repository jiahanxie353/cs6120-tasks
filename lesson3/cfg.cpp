#include "cfg.hpp"

#include <set>
#include <vector>

#include "utils.hpp"

std::set<std::string> TERMINATOR_OPS = {"jmp", "br", "ret"};

bool isTerminator(const Instr* instr) {
    return (instr->contains("labels") && (TERMINATOR_OPS.find(instr->operator[](
                                              "op")) != TERMINATOR_OPS.end()));
}

std::vector<Block> formBasicBlocks(const std::vector<Instr*>& instrs) {
    std::vector<Block> basicBlocks;
    Block curBlock;
    for (const auto& instr : instrs) {
        if (isTerminator(instr)) {
            curBlock.push_back(instr);
            basicBlocks.push_back(curBlock);
            curBlock.clear();
        } else if (instr->contains("label")) {
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

std::vector<Block> genAllBlocks(json& brilProg) {
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
    return allBlocks;
}

int main(int argc, char* argv[]) {
    json brilProg = readJson(argv[1]);
    std::vector<Block> allBlocks = genAllBlocks(brilProg);
    for (const auto& block : allBlocks) {
        std::cout << "[" << std::endl;
        for (const auto instr : block) {
            std::cout << instr->dump() << std::endl;
        }
        std::cout << "]" << std::endl;
    }
    return EXIT_SUCCESS;
}