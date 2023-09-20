#include "cfg.hpp"

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

set<string> TERMINATOR_OPS = {"jmp", "br", "ret"};

bool isTerminator(Instr *instr) {
    return (instr->contains("labels") && (TERMINATOR_OPS.find(instr->operator[](
                                              "op")) != TERMINATOR_OPS.end()));
}

CFG::CFG(json &brilFcn) {
    this->rawBrilFcn = brilFcn;

    vector<Instr *> currInstrs;
    for (auto &instr : brilFcn.at("instrs")) {
        currInstrs.push_back(&instr);
    }

    this->basicBlocks = CFG::buildBlocks(currInstrs);

    this->nameBlocks(this->basicBlocks);

    this->cfg = this->buildCFG(this->getBasicBlocks());
}

vector<shared_ptr<Block>> CFG::buildBlocks(vector<Instr *> &instrs) {
    vector<shared_ptr<Block>> allBlocks;
    vector<Instr *> curBlock;
    for (const auto instr : instrs) {
        if (isTerminator(instr)) {
            curBlock.push_back(instr);
            allBlocks.push_back(std::make_shared<Block>(curBlock));
            allBlocks.back()->computeDefVars();
            curBlock.clear();
        } else if (instr->contains("label")) {
            if (curBlock.size() > 0) {
                allBlocks.push_back(std::make_shared<Block>(curBlock));
                allBlocks.back()->computeDefVars();
            }
            curBlock = {instr};
        } else
            curBlock.push_back(instr);
    }
    if (curBlock.size() > 0) {
        allBlocks.push_back(std::make_shared<Block>(curBlock));
        allBlocks.back()->computeDefVars();
    }
    return allBlocks;
}

void CFG::nameBlocks(vector<shared_ptr<Block>> basicBlocks) {
    int mapSize = 0;
    for (auto block : basicBlocks) {
        string blockName;
        if (block->getInstrs()[0]->contains("label"))
            blockName = (block->getInstrs()[0])->at("label").get<string>();
        else
            blockName = "b" + std::to_string(mapSize);
        block->setLabel(blockName);
        this->label2Block.insert({blockName, block});
        mapSize += 1;
    }
}

map<string, vector<string>>
CFG::buildCFG(vector<shared_ptr<Block>> basicBlocks) {
    map<string, vector<string>> cfgMap;
    int blockCount = 0;
    for (auto block : basicBlocks) {
        Instr *lastInstr = block->getInstrs().back();
        vector<string> successors;
        if (lastInstr->contains("op") && lastInstr->at("op") == "ret") {
            // std::cout << lastInstr->dump() << std::endl;
            // std::cout << "return!" << std::endl;
            successors = {};
        } else if (isTerminator(lastInstr)) {
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
        blockCount += 1;
    }
    this->built = true;
    return cfgMap;
}

int CFG::getSize() const { return basicBlocks.size(); }

vector<shared_ptr<Block>> CFG::getBasicBlocks() const {
    return this->basicBlocks;
}

shared_ptr<Block> CFG::getEntry() const {
    if (this->built) {
        for (auto block : this->getBasicBlocks()) {
            if (block->getPredecessors().size() == 0)
                return block;
            throw std::runtime_error("CFG entry not found!");
        }
    } else
        throw std::runtime_error("CFG not built yet!");
}

map<string, vector<string>> CFG::getCFG() const { return this->cfg; }

bool isDot(char c) { return c == '.'; }

void CFG::visualize() {
    std::cout << "digraph " << rawBrilFcn.at("name").dump() << " {"
              << std::endl;
    for (const auto label : label2Block) {
        string labelName = label.first;
        std::replace_if(labelName.begin(), labelName.end(), isDot, '_');
        std::cout << "  " << labelName << std::endl;
    }
    for (const auto [label, succs] : cfg) {
        string labelName = label;
        std::replace_if(labelName.begin(), labelName.end(), isDot, '_');
        for (const auto succ : succs) {
            string succName = succ;
            std::replace_if(succName.begin(), succName.end(), isDot, '_');
            std::cout << "  " << labelName << " -> " << succName << std::endl;
        }
    }
    std::cout << "}" << std::endl;
}