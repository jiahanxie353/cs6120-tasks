#pragma once

#include <queue>
#include <utility>

#include "../block.hpp"
#include "../cfg.hpp"

std::set<std::pair<std::string, std::string>>
findAllEdges(shared_ptr<Block> rootBlock, std::map<std::string, bool> visited);

std::set<std::pair<std::string, std::string>> findBackEdges(CFG &cfg);

std::set<std::set<std::string>> findNatLoops(CFG &cfg);