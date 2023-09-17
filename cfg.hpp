#pragma once

#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <vector>

#include "block.hpp"
#include "utils.hpp"

class CFG {
   public:
    // build a cfg out of a json-represented bril function
    CFG(json& brilFcn);
    // returns the full, parsed, json-represented bril program
    json getFullFcn() const;
    // returns all basic blocks (list of lists of instructions) of this cfg
    vector<shared_ptr<Block>> getBasicBlocks() const;
    // return the block with label name `label` in this cfg
    shared_ptr<Block> getBlockByLabel(const string label) const;
    // returns the CFG of this function
    map<string, vector<string>> getCFG() const;
    // get the entry of this cfg
    shared_ptr<Block> getEntry() const;

   private:
    // raw json representation of this function's cfg
    json rawBrilFcn;
    // indicate whether the cfg has been built or not
    bool built = false;
    // all basic blocks in this cfg
    vector<shared_ptr<Block>> basicBlocks;
    // CFG, a map of label: list of labels/successors
    map<string, vector<string>> cfg;
    map<string, shared_ptr<Block>> label2Block;
    // build basic blocks from all instructions inside a function (each block's
    // label is not yet unassigned)
    vector<shared_ptr<Block>> buildBlocks(vector<Instr*>&);
    // name each block with a corresponding label for this cfg
    void nameBlocks(vector<shared_ptr<Block>>);
    // build CFG from a list of basic block
    map<string, vector<string>> buildCFG(vector<shared_ptr<Block>>);
};
