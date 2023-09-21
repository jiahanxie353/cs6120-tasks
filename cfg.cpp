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

    basicBlocks = CFG::buildBlocks(currInstrs);

    nameBlocks(basicBlocks);

    cfg = buildCFG(getBasicBlocks());
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

    for (auto block : getBasicBlocks()) {
        if (block->getPredecessors().size() == 0)
            entry = block;

        if (block->getSuccessors().size() == 0)
            exits.push_back(block);
    }

    if (exits.size() > 1) {
        exitSink = std::make_shared<Block>("exit_sink");
        for (auto exit : exits) {
            exit->addSuccessor(exitSink);
            exitSink->addSuccessor(exit);
            cfgMap[exit->getLabel()] = {exitSink->getLabel()};
        }
    } else {
        assert(exits.size() == 1);
        exitSink = exits[0];
    }

    built = true;

    return cfgMap;
}

int CFG::getSize() const {
    if (!built)
        return basicBlocks.size();
    else
        return basicBlocks.size() + 1;
}

vector<shared_ptr<Block>> CFG::getBasicBlocks() const { return basicBlocks; }

shared_ptr<Block> CFG::getEntry() const {
    if (!built)
        return entry;
    throw std::runtime_error("CFG not built yet, don't have an entry!");
}

vector<shared_ptr<Block>> CFG::getExists() const { return exits; }
shared_ptr<Block> CFG::getExitSink() const { return exitSink; }

map<string, vector<string>> CFG::getCFG() const { return cfg; }

void CFG::visualize() {
    std::cout << "digraph " << rawBrilFcn.at("name").dump() << " {"
              << std::endl;
    auto isDot = [](char c) { return c == '.'; };
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