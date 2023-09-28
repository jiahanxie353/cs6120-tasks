#pragma once
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json readJson(std::string);

std::ofstream genOutFile(std::string, std::string);

bool removeNullValues(json &);

// currently only support value operations
json *instrToJson(std::string op, std::string dest,
                  std::vector<std::string> args, std::string varType);