#include "block.hpp"

Block::~Block() {
    for (Instr *instr : instructions) {
        delete instr;
    }
}

void Block::computeDefVars() {
    for (const auto instr : getInstrs()) {
        if (instr->contains("dest"))
            definedVars.insert(instr->at("dest").get<string>());
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

vector<Instr *> Block::getInstrs() const { return instructions; }

void Block::insertInstr(Instr *instr, int pos) {
    instructions.insert(instructions.begin() + pos, instr);
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
    for (auto it = predecessors.begin(); it != predecessors.end(); ++it) {
        if ((*it)->getLabel() == block->getLabel()) {
            predecessors.erase(it);
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
