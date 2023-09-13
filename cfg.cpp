#include "cfg.hpp"

#include <set>
#include <string>
#include <vector>

set<string> TERMINATOR_OPS = {"jmp", "br", "ret"};

bool isTerminator(Instr* instr) {
    return (instr->contains("labels") && (TERMINATOR_OPS.find(instr->operator[](
                                              "op")) != TERMINATOR_OPS.end()));
}

CFG::CFG(json& brilJson) {
    for (auto& fcn : brilJson.at("functions")) {
        vector<Instr*> currInstrs;
        for (auto& instr : fcn.at("instrs")) {
            currInstrs.push_back(&instr);
        }
        vector<Block> currBlock = CFG::buildFcnBlocks(currInstrs);
        this->basicBlocks.insert({fcn["name"].get<string>(), currBlock});

        this->nameBlocks(fcn["name"].get<string>());

        for (auto& basicBlock : this->basicBlocks.at(fcn["name"])) {
            std::cout << basicBlock.getLabel() << std::endl;
        }
    }
}

vector<Block>& CFG::getBasicBlocks(string fcnName) const {
    return this->basicBlocks.at(fcnName);
}

vector<Block> CFG::buildFcnBlocks(vector<Instr*>& instrs) {
    std::cout << "hello" << std::endl;
    vector<Block> basicBlocks;
    vector<Instr*> curBlock;
    for (const auto instr : instrs) {
        if (isTerminator(instr)) {
            curBlock.push_back(instr);
            basicBlocks.push_back(Block(curBlock));
            curBlock.clear();
        } else if (instr->contains("label")) {
            if (curBlock.size() > 0) {
                basicBlocks.push_back(Block(curBlock));
            }
            curBlock = {instr};
        } else
            curBlock.push_back(instr);
    }
    if (curBlock.size() > 0) {
        basicBlocks.push_back(Block(curBlock));
    }
    return basicBlocks;
}

void CFG::nameBlocks(const string fcnName) {
    vector<Block>& fcnBlocks = this->getBasicBlocks(fcnName);
    int mapSize = 0;
    for (auto& block : fcnBlocks) {
        string blockName;
        if (block.getInstrs()[0]->contains("label")) {
            blockName = (block.getInstrs()[0])->at("label").get<string>();
        } else
            blockName = "b" + std::to_string(mapSize);
        block.setLabel(blockName);
        mapSize += 1;
    }
}

std::map<std::string, std::vector<std::string>> buildCFG(
    const std::map<std::string, Block> label2Block) {}

const vector<Instr*>& Block::getInstrs() const { return this->instructions; }

string Block::getLabel() const { return this->label; }

void Block::setLabel(string labelName) { this->label = labelName; }
