#include "form_blocks.hpp"

#include <set>
#include <vector>

#include "../utils.h"

std::set<std::string> TERMINATOR_OPS = {"jmp", "br", "ret"};

bool isTerminator(const Instr *instr) {
    return (instr->contains("labels") && (TERMINATOR_OPS.find(instr->operator[](
                                              "op")) != TERMINATOR_OPS.end()));
}

std::vector<Block> formBasicBlocks(const std::vector<Instr *> &instrs) {
    std::vector<Block> basicBlocks;
    Block curBlock;
    for (const auto &instr : instrs) {
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

std::vector<Block> genAllBlocks(json &brilProg) {
    std::vector<Block> allBlocks;
    for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
        std::vector<Instr *> brilInstrs;
        for (auto &instr : brilProg["functions"][fcnIdx]["instrs"]) {
            brilInstrs.push_back(&instr);
        }
        std::vector<Block> blocks = formBasicBlocks(brilInstrs);
        for (const auto &block : blocks) {
            allBlocks.push_back(block);
        }
    }
    return allBlocks;
}

std::tuple<std::vector<Block>, std::vector<std::vector<bool>>,
           std::vector<std::set<Var>>>
genBlocksOverwrites(json &brilProg) {
    std::vector<Block> allBlocks;
    std::vector<std::vector<bool>> allOverwrites;
    std::vector<std::set<Var>> allVarNames;
    for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
        std::vector<Instr *> brilInstrs;
        for (auto &instr : brilProg["functions"][fcnIdx]["instrs"]) {
            brilInstrs.push_back(&instr);
        }
        std::vector<Block> blocks = formBasicBlocks(brilInstrs);
        for (const auto &block : blocks) {
            allBlocks.push_back(block);
        }

        for (const auto &block : blocks) {
            std::vector<bool> curBlockOverwrite(block.size(), false);
            std::set<Var> seenDest;
            for (auto it = block.rbegin(); it != block.rend(); ++it) {
                if ((*it)->contains("dest")) {
                    std::string destName = (*it)->at("dest");
                    if (seenDest.find(destName) != seenDest.end()) {
                        size_t index = block.rend() - it - 1;
                        curBlockOverwrite[index] = true;
                    }
                    seenDest.insert(destName);
                }
            }
            allOverwrites.push_back(curBlockOverwrite);
            allVarNames.push_back(seenDest);
        }
    }
    return std::make_tuple(allBlocks, allOverwrites, allVarNames);
}
