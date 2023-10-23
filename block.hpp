#pragma once

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <tuple>
#include <vector>

#include "utils.h"

using std::map;
using std::set;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

using json = nlohmann::json;

namespace bril {
using Var = string;
using Op = string;
using Instr = json;
} // namespace bril

using namespace bril;

class Block {
public:
  Block(string label) : label(label){};
  Block(vector<Instr *> instrs) : instructions(instrs){};
  // returns the label of this basic block
  string getLabel() const;
  // returns all instructions of this basic block
  vector<Instr *> getInstrs() const;
  // returns the predecessors of this basic block
  vector<shared_ptr<Block>> getPredecessors() const;
  // returns the successors of this basic block
  vector<shared_ptr<Block>> getSuccessors() const;
  bool hasField(string, string) const;
  void removeInstr(int pos);

  template <class T> set<T> getDefined() const {
    if constexpr (std::is_same<T, string>::value) {
      set<string> res;
      for (const auto instr : getInstrs()) {
        if (instr->contains("dest"))
          res.insert(instr->at("dest").get<string>());
      }
      return res;
    } else if constexpr (std::is_same<T, Instr *>::value) {
      set<Instr *> res;
      for (const auto instr : getInstrs()) {
        if (instr->contains("dest"))
          res.insert(instr);
      }
      return res;
    }
  }

  template <class T> set<T> computeKilled(const set<T> &inputs) const {
    if constexpr (std::is_same<T, Var>::value) {
      set<Var> currAvailVars = inputs;
      set<Var> killedVars;
      for (const auto instr : getInstrs()) {
        if (instr->contains("dest")) {
          Var destVar = instr->at("dest");
          if (currAvailVars.find(destVar) != currAvailVars.end())
            killedVars.insert(destVar);
          else
            currAvailVars.insert(destVar);
        }
      }
      return killedVars;
    } else if (std::is_same<T, Instr *>::value) {
      set<Instr *> currAvailDefs = inputs;
      set<Instr *> killedDefs;
      for (const auto instr : getInstrs()) {
        if (instr->contains("dest")) {
          string destVar = instr->at("dest").get<string>();
          for (const auto availDef : currAvailDefs) {
            if (!(availDef->contains("dest"))) {
              assert(availDef->contains("name"));
              if (availDef->at("name").get<string>() == destVar)
                killedDefs.insert(availDef);
            } else if (availDef->at("dest").get<string>() == destVar)
              killedDefs.insert(availDef);
            else
              currAvailDefs.insert(instr);
          }
        }
      }
      return killedDefs;
    }
  }

  void insertInstr(Instr *, int);

  void setLabel(const string);

  void computeDefVars();

  void addPredecessor(shared_ptr<Block>);
  void addSuccessor(shared_ptr<Block>);
  void removePredecessor(shared_ptr<Block>);
  void removeSuccessor(shared_ptr<Block>);

private:
  friend class CFG;

  string label;
  vector<Instr *> instructions;
  vector<shared_ptr<Block>> predecessors;
  vector<shared_ptr<Block>> successors;

  set<Var> definedVars;
};