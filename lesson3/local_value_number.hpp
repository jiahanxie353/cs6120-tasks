#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "../utils.hpp"
#include "form_blocks.hpp"

using Args = std::vector<Var>;
using ExprTuple = std::tuple<Op, std::vector<int>>;

json lvn(json);