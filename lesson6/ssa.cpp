#include "ssa.h"
#include "../utils.h"
#include <stack>

using stack = std::stack<string>;

void addPhiNode(shared_ptr<Block> block, string var, string varType) {
    vector<string> args(2 * block->getPredecessors().size(), var);
    for (int pI = 0; pI < 2 * block->getPredecessors().size(); pI += 2) {
        args[pI] = block->getPredecessors()[pI / 2]->getLabel();
    }

    Instr *instr = instrToJson("phi", var, args, varType);
    block->insertInstr(instr, 1);
}

// insert Phi nodes into `cfg`
void insertPhiNodes(CFG &cfg) {
    map<string, set<shared_ptr<Block>>> defsites;
    map<shared_ptr<Block>, set<string>> orig;
    map<string, string> varType;
    // find the set of all blocks that define each variable;
    // and the set of all variables that are defined in each block
    for (const auto &block : cfg.getBasicBlocks()) {
        for (const auto &var : block->getDefined<string>()) {
            orig[block].insert(var);
            defsites[var].insert(block);
            vector<Instr *> instrs = block->getInstrs();
            for (const auto instr : instrs) {
                if (instr->contains("dest") &&
                    (instr->at("dest").get<string>() == var)) {
                    varType[var] = instr->at("type").get<string>();
                    break;
                }
            }
        }
    }

    map<string, set<string>> Phi;
    for (const auto &var : cfg.getAllVars()) {
        auto &def = defsites[var];
        bool insertPhi = def.size() > 1;
        while (!def.empty()) {
            auto blockIt = def.begin();
            def.erase(blockIt);
            if (insertPhi) {
                for (auto &frontLabel :
                     cfg.getDomFrontier((*blockIt)->getLabel())) {
                    if (Phi[var].find(frontLabel) == Phi[var].end()) {
                        auto frontBlock = cfg.getBlock(frontLabel);
                        addPhiNode(frontBlock, var, varType[var]);
                        Phi[var].insert(frontLabel);
                        if (orig[frontBlock].find(var) ==
                            orig[frontBlock].end())
                            def.insert(frontBlock);
                    }
                }
            }
        }
    }
}

void rename(CFG &cfg, shared_ptr<Block> block, map<string, stack> &stackMap,
            map<string, int> &varCount, map<string, string> &origName) {
    map<string, int> pushed;
    for (auto &instr : block->getInstrs()) {
        // replace each argument to instr w/ stack[old name]
        if (instr->contains("args")) {
            for (auto &arg : instr->at("args")) {
                string orig = origName[arg.get<string>()];
                if (orig == "")
                    continue;
                arg = stackMap[orig].top();
            }
        }

        // replace instr's destination with a new name
        if (instr->contains("dest")) {
            string orig = origName[instr->at("dest").get<string>()];
            string newDest = orig + std::to_string(varCount[orig]);
            varCount[orig] += 1;
            instr->at("dest") = newDest;
            origName[newDest] = orig;
            // push that new name onto stack[old name]
            stackMap[orig].push(newDest);
            pushed[orig] += 1;
        }
    }

    for (auto &succ : block->getSuccessors()) {
        for (auto &instr : succ->getInstrs()) {
            if (instr->contains("op") &&
                instr->at("op").get<string>() == "phi") {
                for (auto it = instr->at("args").begin();
                     it != instr->at("args").end(); ++it) {
                    if (it->get<string>() == block->getLabel()) {
                        string orig = origName[(it + 1)->get<string>()];
                        *(it + 1) = stackMap[orig].top();
                        break;
                    }
                }
            }
        }
    }

    for (auto &immChild : cfg.getImmDominatees(block->getLabel()))
        rename(cfg, cfg.getBlock(immChild), stackMap, varCount, origName);

    for (auto &[orig, newPushed] : pushed) {
        while (newPushed > 0) {
            newPushed -= 1;
            stackMap[orig].pop();
        }
    }
}

// rename variables inside `cfg`
void renameVars(CFG &cfg) {
    map<string, stack> stackMap;
    map<string, int> varCount;
    map<string, string> origName;
    for (const auto &var : cfg.getAllVars()) {
        stack s;
        s.push(var);
        stackMap[var] = s;
        varCount[var] += 1;

        origName[var] = var;
    }

    rename(cfg, cfg.getEntry(), stackMap, varCount, origName);
}

json &toSSA(json &brilFcn) {
    CFG cfg(brilFcn);

    insertPhiNodes(cfg);
    renameVars(cfg);

    return cfg.getBrilJson();
}