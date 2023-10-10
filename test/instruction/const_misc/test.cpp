#include <assert.h>
#include <iostream>
#include <map>
#include <set>

#include "../../../instr.h"
#include "../../../utils.h"

using std::map;
using std::string;
using std::vector;

std::set<string> MiscOps = {"id", "print"};

int main(int argc, char *argv[]) {
  json constJson = readJson(argv[1]);

  for (auto &brilFcn : constJson.at("functions")) {
    map<string, Instruction *> allInstrMap;
    map<string, ValueInstr *> valInstrMap;
    map<string, EffectInstr *> effInstrMap;
    map<string, ArgValue *> argValMap;
    for (auto &brilInstr : brilFcn.at("instrs")) {
      auto op = brilInstr.at("op").get<string>();
      if (op == "const") {
        allInstrMap[brilInstr.at("dest").get<string>()] =
            new ConstInstr(&brilInstr);
      } else if (op == "id") {
        auto args = brilInstr.at("args").get<vector<string>>();
        auto dest = brilInstr.at("dest").get<string>();

        vector<ArgValue *> currArgs;
        for (const auto &arg : args) {
          std::cout << allInstrMap.at(arg)->dump() << std::endl;
          if (argValMap.find(arg) != argValMap.end())
            delete argValMap.at(arg);
          argValMap[arg] = new ArgValue(allInstrMap.at(arg)->getValue());
          currArgs.push_back(argValMap.at(arg));
        }

        valInstrMap[dest] = new ValueInstr(
            &brilInstr, std::make_optional<vector<ArgValue *>>(currArgs));

        allInstrMap[dest] = valInstrMap.at(dest);

        std::cout << valInstrMap.at(dest)->dump() << std::endl;
      } else if (op == "print") {
        if (brilInstr.contains("args")) {
          auto args = brilInstr.at("args").get<vector<string>>();

          vector<ArgValue *> currArgs;
          for (const auto &arg : args) {
            std::cout << allInstrMap.at(arg)->dump() << std::endl;
            if (argValMap.find(arg) != argValMap.end())
              delete argValMap.at(arg);
            argValMap[arg] = new ArgValue(allInstrMap.at(arg)->getValue());
            currArgs.push_back(argValMap.at(arg));
          }

          if (effInstrMap.find(op) != effInstrMap.end())
            delete effInstrMap.at(op);

          effInstrMap[op] = new EffectInstr(
              &brilInstr, std::make_optional<vector<ArgValue *>>(currArgs));

          allInstrMap[op] = effInstrMap.at(op);

          std::cout << effInstrMap.at(op)->dump() << std::endl;
        }
      }
    }
    for (auto [k, v] : allInstrMap)
      delete v;
    for (auto [k, v] : argValMap)
      delete v;
  }

  return EXIT_SUCCESS;
}