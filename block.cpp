#include "block.hpp"

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
