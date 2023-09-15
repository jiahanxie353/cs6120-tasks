#include "block.hpp"

vector<Instr*> Block::getInstrs() const { return this->instructions; }

string Block::getLabel() const { return this->label; }

void Block::setLabel(string labelName) { this->label = labelName; }

void Block::addPredecessor(Block* block) {
    this->predecessors.push_back(block);
}

void Block::addSuccessor(Block* block) { this->successors.push_back(block); }

vector<Block*> Block::getPredecessors() const { return this->predecessors; }

vector<Block*> Block::getSuccessors() const { return this->successors; }