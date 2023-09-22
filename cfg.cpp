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
        basicBlocks.push_back(exitSink);
        for (auto exit : exits) {
            exit->addSuccessor(exitSink);
            exitSink->addSuccessor(exit);
            cfgMap[exit->getLabel()] = {exitSink->getLabel()};
        }
    } else {
        assert(exits.size() == 1);
        exitSink = exits[0];
    }
    cfgMap[exitSink->getLabel()] = {};

    built = true;

    return cfgMap;
}

set<string> multipleSetsIntersect(vector<set<string>> allSets) {
    // find the set with the smallest size, make that as the starting point
    set<string> smallest;
    int smallestSize = INT16_MAX;
    for (const auto currSet : allSets) {
        if (currSet.size() < smallestSize) {
            smallest = currSet;
            smallestSize = currSet.size();
        }
    }
    // iterate through all sets, take intersect between that smallest and the
    // current set, update the smallest set in-place
    for (const auto currSet : allSets) {
        set<string>::iterator it1 = smallest.begin();
        set<string>::iterator it2 = currSet.begin();
        while ((it1 != smallest.end()) && (it2 != currSet.end())) {
            if (*it1 < *it2) {
                smallest.erase(it1++);
            } else if (*it2 < *it1) {
                ++it2;
            } else { // *it1 == *it2
                ++it1;
                ++it2;
            }
        }
        // Anything left in set_1 from here on did not appear in set_2,
        // so we remove it.
        smallest.erase(it1, smallest.end());
    }

    return smallest;
}

set<string> CFG::computeDominators(string dominatee) {
    // map from vertices to every vertice
    map<string, set<string>> dom;
    map<string, set<string>> prevDom = dom;
    for (const auto src : getBasicBlocks()) {
        set<string> curSrcSet;
        for (const auto dest : getBasicBlocks()) {
            curSrcSet.insert(dest->getLabel());
        }
        dom[src->getLabel()] = curSrcSet;
    }
    string entry = getEntry()->getLabel();
    dom[entry] = {entry};

    while (prevDom != dom) {
        prevDom = dom;
        for (const auto block : getBasicBlocks()) {
            if (block->getLabel() != entry) {
                vector<set<string>> predDoms;
                for (const auto pred : block->getPredecessors())
                    predDoms.push_back(dom[pred->getLabel()]);
                set<string> predDomIntersect = multipleSetsIntersect(predDoms);

                set<string> newDom = {block->getLabel()};
                newDom.insert(predDomIntersect.begin(), predDomIntersect.end());

                dom[block->getLabel()] = newDom;
            }
        }
    }

    return dom[dominatee];
}

bool CFG::contains(const string blockLabel) const {
    for (const auto block : getBasicBlocks()) {
        if (block->getLabel() == blockLabel)
            return true;
    }
    return false;
}

shared_ptr<Block> CFG::getBlock(const string blockLabel) const {
    if (!contains(blockLabel))
        throw std::runtime_error("Block not found!");
    return label2Block.at(blockLabel);
}

int CFG::getSize() const {
    if (!built)
        return basicBlocks.size();
    else
        return basicBlocks.size() + 1;
}

vector<shared_ptr<Block>> CFG::getBasicBlocks() const { return basicBlocks; }

shared_ptr<Block> CFG::getEntry() const {
    if (built)
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