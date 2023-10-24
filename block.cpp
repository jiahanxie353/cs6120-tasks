#include "block.hpp"

void Block::computeDefVars() {
  for (const auto instr : getInstrs()) {
    if (instr->contains("dest"))
      definedVars.insert(instr->at("dest").get<string>());
  }
}

bool Block::hasField(string fieldName, string value) const {
  for (const auto instr : getInstrs()) {
    if (instr->contains(fieldName) && (instr->at(fieldName).dump() == value))
      return true;
  }
  return false;
}

vector<Instr *> Block::getInstrs() const { return instructions; }

void Block::insertInstr(Instr *instr, int pos) {
  instructions.insert(instructions.begin() + pos, instr);
}

void Block::removeInstr(Instr *instr) {
  for (int pos = 0; pos < instructions.size(); ++pos) {
    if (instructions[pos] == instr)
      removeInstr(pos);
  }
}

void Block::removeInstr(int pos) {
  instructions.erase(instructions.begin() + pos);
}

string Block::getLabel() const { return label; }

vector<shared_ptr<Block>> Block::getPredecessors() const {
  return predecessors;
}

vector<shared_ptr<Block>> Block::getSuccessors() const { return successors; }

void Block::setLabel(string labelName) { label = labelName; }

void Block::addPredecessor(shared_ptr<Block> block) {
  predecessors.push_back(block);
}

void Block::addSuccessor(shared_ptr<Block> block) {
  successors.push_back(block);
}

void Block::removePredecessor(shared_ptr<Block> block) {
  for (auto it = predecessors.begin(); it != predecessors.end();) {
    if ((*it)->getLabel() == block->getLabel()) {
      it = predecessors.erase(it);
    } else {
      ++it;
    }
  }
}

void Block::removeSuccessor(shared_ptr<Block> block) {
  for (auto it = successors.begin(); it != successors.end();) {
    if ((*it)->getLabel() == block->getLabel()) {
      it = successors.erase(it);
    } else {
      ++it;
    }
  }
}
