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
    vector<Block*> getBasicBlocks() const;
    // return the block with label name `label` in this cfg
    Block* getBlockByLabel(const string label) const;
    // returns the CFG of this function
    map<string, vector<string>> getCFG() const;
    // get the entry of this cfg
    Block* getEntry() const;
    ~CFG();

   private:
    // raw json representation of this function's cfg
    json rawBrilFcn;
    // indicate whether the cfg has been built or not
    bool built = false;
    // all basic blocks in this cfg
    vector<Block*> basicBlocks;
    // CFG, a map of label: list of labels/successors
    map<string, vector<string>> cfg;
    map<string, Block*> label2Block;
    // build basic blocks from all instructions inside a function (each block's
    // label is not yet unassigned)
    vector<Block*> buildBlocks(vector<Instr*>&);
    // name each block with a corresponding label for this cfg
    void nameBlocks(vector<Block*>);
    // build CFG from a list of basic block
    map<string, vector<string>> buildCFG(vector<Block*>);
};
