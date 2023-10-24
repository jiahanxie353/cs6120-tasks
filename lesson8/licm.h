#pragma once

#include "../block.hpp"
#include "../cfg.hpp"
#include "../lesson4/data_flow.hpp"
#include "nat_loop.h"

// Inserts loop preheader to this `natLoop` in the corresponding `cfg`
shared_ptr<Block>
insertPreheader(CFG &cfg,
                std::pair<std::string, std::set<std::string>> natLoop);

// Identifies loop-invariant instructions of this `natLoop` based in the
// corresponding `cfg`
map<string, set<Instr *>>
identLoopInvarInstrs(CFG &cfg,
                     tuple<map<shared_ptr<Block>, set<Instr *>>,
                           map<shared_ptr<Block>, set<Instr *>>>
                         reachDef,
                     std::pair<std::string, std::set<std::string>> natLoop);

// Inserts a loop invarant `instr` to `preHeader` in this `natLoop`
void insertToPreHead(CFG &cfg, shared_ptr<Block> preHeader,
                     std::pair<Instr *, string> instrBlock,
                     std::pair<string, set<string>> natLoop);