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

// std::vector<
//     std::tuple<std::vector<std::tuple<Instr*, bool>>, std::set<std::string>>>
// formAugBasicBlocks(const std::vector<Instr*>& instrs) {
//     std::map<Var, int> occurCount;
//     for (const auto instr : instrs) {
//         if (instr->contains("dest")) {
//             std::string destVar = instr->at("dest").dump();
//             auto iter = occurCount.find(destVar);
//             if (iter != occurCount.end())
//                 iter->second += 1;
//             else
//                 occurCount.insert({destVar, 1});
//         }
//     }

//     std::vector<std::tuple<std::vector<std::tuple<Instr*, bool>>,
//                            std::set<std::string>>>
//         res;
//     std::vector<std::tuple<Instr*, bool>> curAugBlock;
//     std::set<std::string> blockVarNames;
//     for (const auto& instr : instrs) {
//         if (isTerminator(instr)) {
//             curAugBlock.push_back(std::make_tuple(instr, false));
//             res.push_back(std::make_tuple(curAugBlock, blockVarNames));
//             curAugBlock.clear();
//             blockVarNames.clear();
//         } else if (instr->contains("label")) {  // the start of the basic
//         block
//             if (curAugBlock.size() > 0) {
//                 res.push_back(std::make_tuple(curAugBlock, blockVarNames));
//             }
//             curAugBlock = {std::make_tuple(instr, false)};
//         } else {
//             if (instr->contains("dest")) {
//                 std::string destName = instr->at("dest").dump();
//                 blockVarNames.insert(destName);
//                 if (occurCount[destName] > 1) {
//                     curAugBlock.push_back(std::make_tuple(instr, true));
//                     occurCount[destName] -= 1;
//                 } else
//                     curAugBlock.push_back(std::make_tuple(instr, false));
//             }
//         }
//     }
//     if (curAugBlock.size() > 0) {
//         res.push_back(std::make_tuple(curAugBlock, blockVarNames));
//     }
//     return res;
// }

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

std::tuple<std::vector<Block>, std::vector<std::vector<bool>>>
genBlocksOverwrite(json& brilProg) {
    std::vector<Block> allBlocks;
    std::vector<std::vector<bool>> allOverwrite;
    for (int fcnIdx = 0; fcnIdx < brilProg["functions"].size(); ++fcnIdx) {
        std::vector<Instr*> brilInstrs;
        for (auto& instr : brilProg["functions"][fcnIdx]["instrs"]) {
            brilInstrs.push_back(&instr);
        }
        std::vector<Block> blocks = formBasicBlocks(brilInstrs);
        for (const auto& block : blocks) {
            allBlocks.push_back(block);
        }

        for (const auto& block : blocks) {
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
            allOverwrite.push_back(curBlockOverwrite);
        }
    }
    return std::make_tuple(allBlocks, allOverwrite);
}

int main(int argc, char* argv[]) {
    json brilProg = readJson(argv[1]);
    std::tuple<std::vector<Block>, std::vector<std::vector<bool>>> allBlocks =
        genBlocksOverwrite(brilProg);
    std::vector<Block> blocks = std::get<0>(allBlocks);
    std::vector<std::vector<bool>> willBeOverwritten = std::get<1>(allBlocks);
    for (size_t i = 0; i < blocks.size(); ++i) {
        std::cout << "[" << std::endl;
        for (size_t j = 0; j < blocks[i].size(); ++j) {
            std::cout << blocks[i][j]->dump() << std::endl;
            std::cout << "willBeOverwritten: " << willBeOverwritten[i][j]
                      << std::endl;
        }
        std::cout << "]" << std::endl;
    }
    return EXIT_SUCCESS;
}