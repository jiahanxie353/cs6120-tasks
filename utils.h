#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Reads the file with `filename` and returns a pointer to a json object
nlohmann::json readJson(std::string filename);

std::ofstream genOutFile(std::string, std::string);

bool removeNullValues(json &);

// currently only support value operations
nlohmann::json *instrToJson(std::string op, std::string dest,
                            std::vector<std::string> args, std::string varType);