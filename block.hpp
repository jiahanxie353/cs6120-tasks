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
using std::shared_ptr;
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
    vector<shared_ptr<Block>> getPredecessors() const;
    // returns the successors of this basic block
    vector<shared_ptr<Block>> getSuccessors() const;

    template <class T>
    set<T> getDefined() const {
        if (std::is_same<T, Var>::value) {
            return this->definedVars;
        }
    }

    template <class T>
    set<T> computeKilled(const set<T>& inputs) const {
        if (std::is_same<T, Var>::value) {
            set<Var> currAvailVars = inputs;
            set<Var> killedVars;
            for (const auto instr : this->getInstrs()) {
                if (instr->contains("dest")) {
                    Var destVar = instr->at("dest");
                    if (currAvailVars.find(destVar) != currAvailVars.end())
                        killedVars.insert(destVar);
                    else
                        currAvailVars.insert(destVar);
                }
            }
            return killedVars;
        }
    }

   private:
    friend class CFG;

    void setLabel(const string);
    void addPredecessor(shared_ptr<Block>);
    void addSuccessor(shared_ptr<Block>);

    void computeDefVars();

    string label;
    vector<Instr*> instructions;
    vector<shared_ptr<Block>> predecessors;
    vector<shared_ptr<Block>> successors;

    set<Var> definedVars;
};