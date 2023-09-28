#include "ssa.h"
#include "../utils.h"

void addPhiNode(shared_ptr<Block> block, string var, string varType) {
    vector<string> args(block->getPredecessors().size(), var);
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
        while (!def.empty()) {
            auto blockIt = def.begin();
            def.erase(blockIt);
            for (auto &frontLabel :
                 cfg.getDomFrontier((*blockIt)->getLabel())) {
                if (Phi[var].find(frontLabel) == Phi[var].end()) {
                    auto frontBlock = cfg.getBlock(frontLabel);
                    addPhiNode(frontBlock, var, varType[var]);
                    Phi[var].insert(frontLabel);
                    if (orig[frontBlock].find(var) == orig[frontBlock].end())
                        def.insert(frontBlock);
                }
            }
        }
    }

    for (const auto &block : cfg.getBasicBlocks()) {
        for (const auto &instr : block->getInstrs()) {
            std::cout << instr->dump() << std::endl;
        }
    }
}

// rename variables inside `cfg`
void renameVars(CFG &cfg) {}

json &toSSA(json &brilFcn) {
    CFG cfg(brilFcn);

    insertPhiNodes(cfg);
    renameVars(cfg);

    return cfg.getBrilJson();
}