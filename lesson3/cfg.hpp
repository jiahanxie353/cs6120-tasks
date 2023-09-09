#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <set>
#include <vector>

using json = nlohmann::json;

namespace bril {
using Var = std::string;
using Op = std::string;
using Instr = json;
using Block = std::vector<Instr*>;
}  // namespace bril

using namespace bril;

std::vector<Block> genAllBlocks(json&);
std::tuple<std::vector<Block>, std::vector<std::vector<bool>>>
genBlocksOverwrite(json&);