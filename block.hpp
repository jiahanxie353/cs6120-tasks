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
    vector<Instr*> getInstrs() const;
    // returns the predecessors of this basic block
    vector<Block*> getPredecessors() const;
    // returns the successors of this basic block
    vector<Block*> getSuccessors() const;

    set<Var> getDefinedVars() const;
    set<Var> getKilledVars() const;

   private:
    friend class CFG;
    void setLabel(const string);
    void addPredecessor(Block*);
    void addSuccessor(Block*);
    void computeDefVars();
    void computeKilledVars(const set<Var> varsInput);

    string label;
    vector<Instr*> instructions;
    vector<Block*> predecessors;
    vector<Block*> successors;

    set<Var> definedVars;
    set<Var> killedVars;
};