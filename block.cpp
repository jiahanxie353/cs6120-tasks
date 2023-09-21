#include "block.hpp"

void Block::computeDefVars() {
    for (const auto instr : this->getInstrs()) {
        if (instr->contains("dest"))
            this->definedVars.insert(instr->at("dest").get<string>());
    }
}

bool Block::hasField(string fieldName, string value) const {
    for (const auto instr : getInstrs()) {
        if (instr->contains(fieldName) &&
            (instr->at(fieldName).dump() == value))
            return true;
    }
    return false;
}

vector<Instr *> Block::getInstrs() const { return this->instructions; }

string Block::getLabel() const { return label; }

vector<shared_ptr<Block>> Block::getPredecessors() const {
    return this->predecessors;
}

vector<shared_ptr<Block>> Block::getSuccessors() const {
    return this->successors;
}

void Block::setLabel(string labelName) { this->label = labelName; }

void Block::addPredecessor(shared_ptr<Block> block) {
    this->predecessors.push_back(block);
}

void Block::addSuccessor(shared_ptr<Block> block) {
    this->successors.push_back(block);
}
