#pragma once
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json readJson(std::string);

std::ofstream genOutFile(std::string, std::string);