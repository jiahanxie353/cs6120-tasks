#pragma once

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <tuple>
#include <vector>

#include "utils.hpp"

using std::map;
using std::set;
using std::string;
using std::tuple;
using std::vector;

using json = nlohmann::json;

namespace bril {
using Var = string;
using Op = string;
using Instr = json;
}  // namespace bril

using namespace bril;

class Block {
   public:
    Block(vector<Instr*> instrs) : instructions(instrs){};
    // returns the label of this basic block
    string getLabel() const;
    // returns all instructions of this basic block
    const vector<Instr*>& getInstrs() const;
    // returns the predecessors of this basic block
    vector<Block*>& getPredecessors() const;
    // returns the successors of this basic block
    vector<Block*>& getSuccessors() const;

   private:
    friend class CFG;
    void setLabel(const string);
    void addPredecessor(Block*);
    void addSuccessor(Block*);

    string label;
    vector<Instr*> instructions;
    vector<Block*> predecessors;
    vector<Block*> successors;
};

class CFG {
   public:
    // build a cfg out of a json-represented bril program
    CFG(json& j);
    // returns the full, parsed, json-represented bril program
    json getFullProg() const;
    // returns all basic blocks (list of lists of instructions) of a function in
    // this cfg
    vector<Block>& getBasicBlocks(string) const;
    // returns the CFG (a map of label: list of labels/successors)
    map<string, vector<string>>& getCFG() const;

   private:
    json rawBril;
    // mapping from function names to their basic blocks
    map<string, vector<Block>&> basicBlocks;
    map<string, vector<string>> cfg;
    // build basic blocks from all instructions inside a function (each block's
    // label is not yet unassigned)
    vector<Block> buildFcnBlocks(vector<Instr*>&);
    // name this[fcn]->label2Block
    void nameBlocks(string);
};
