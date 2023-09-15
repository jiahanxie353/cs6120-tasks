#include "cfg.hpp"

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

set<string> TERMINATOR_OPS = {"jmp", "br", "ret"};

bool isTerminator(Instr* instr) {
    return (instr->contains("labels") && (TERMINATOR_OPS.find(instr->operator[](
                                              "op")) != TERMINATOR_OPS.end()));
}

CFG::CFG(json& brilFcn) {
    this->rawBrilFcn = brilFcn;

    vector<Instr*> currInstrs;
    for (auto& instr : brilFcn.at("instrs")) {
        currInstrs.push_back(&instr);
    }

    this->basicBlocks = CFG::buildBlocks(currInstrs);

    this->nameBlocks(this->basicBlocks);

    this->cfg = this->buildCFG(this->getBasicBlocks());

    for (const auto block : this->getBasicBlocks()) {
        std::cout << block->getLabel() + "'s preds are: " << std::endl;
        for (const auto pred : block->getPredecessors()) {
            std::cout << pred->getLabel() << std::endl;
        }
    }

    for (const auto block : this->getBasicBlocks()) {
        std::cout << block->getLabel() + "'s succs are: " << std::endl;
        for (const auto succ : block->getSuccessors()) {
            std::cout << succ->getLabel() << std::endl;
        }
    }
}

vector<Block*> CFG::buildBlocks(vector<Instr*>& instrs) {
    vector<Block*> allBlocks;
    vector<Instr*> curBlock;
    for (const auto instr : instrs) {
        if (isTerminator(instr)) {
            curBlock.push_back(instr);
            allBlocks.push_back(new Block(curBlock));
            curBlock.clear();
        } else if (instr->contains("label")) {
            if (curBlock.size() > 0) {
                allBlocks.push_back(new Block(curBlock));
            }
            curBlock = {instr};
        } else
            curBlock.push_back(instr);
    }
    if (curBlock.size() > 0) {
        allBlocks.push_back(new Block(curBlock));
    }
    return allBlocks;
}

void CFG::nameBlocks(vector<Block*>) {
    int mapSize = 0;
    for (auto block : this->getBasicBlocks()) {
        string blockName;
        if (block->getInstrs()[0]->contains("label")) {
            blockName = (block->getInstrs()[0])->at("label").get<string>();
        } else
            blockName = "b" + std::to_string(mapSize);
        block->setLabel(blockName);
        this->label2Block.insert({blockName, block});
        mapSize += 1;
    }
}

map<string, vector<string>> CFG::buildCFG(vector<Block*> basicBlocks) {
    map<string, vector<string>> cfgMap;
    int blockCount = 0;
    for (auto block : basicBlocks) {
        Instr* lastInstr = block->getInstrs().back();
        vector<string> successors;
        if (isTerminator(lastInstr)) {
            if (lastInstr->at("op") == "jmp" || lastInstr->at("op") == "br")
                successors = lastInstr->at("labels").get<vector<string>>();
        } else {
            if (blockCount < basicBlocks.size() - 1)
                successors = {basicBlocks[blockCount + 1]->getLabel()};
        }
        cfgMap.insert({block->getLabel(), successors});
        for (auto succ : successors) {
            block->addSuccessor(label2Block[succ]);
            label2Block[succ]->addPredecessor(block);
        }
    }
    this->built = true;
    return cfgMap;
}

CFG::~CFG() {
    for (Block* block : this->basicBlocks) {
        delete block;  // Deleting each dynamically allocated Block object
    }
}

vector<Block*> CFG::getBasicBlocks() const { return this->basicBlocks; }

Block* CFG::getEntry() const {
    if (this->built) {
        for (const auto block : this->getBasicBlocks()) {
            if (block->getPredecessors().size() == 0) return block;
        }
    } else
        throw std::runtime_error("CFG not built yet!");
}
