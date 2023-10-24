#pragma once

#include "../block.hpp"
#include "../cfg.hpp"
#include "../lesson4/data_flow.hpp"
#include "nat_loop.h"

// Inserts loop preheader to this `natLoop` in the corresponding `cfg`
void insertPreheader(CFG &cfg,
                     std::pair<std::string, std::set<std::string>> natLoop);

// Identifies loop-invariant instructions of this `natLoop` based in the
// corresponding `cfg`
vector<Instr *>
identLoopInvarInstrs(CFG &cfg,
                     tuple<map<shared_ptr<Block>, set<Instr *>>,
                           map<shared_ptr<Block>, set<Instr *>>>
                         reachDef,
                     std::pair<std::string, std::set<std::string>> natLoop);