#include <assert.h>
#include <iostream>

#include "../../instr.h"
#include "../../utils.h"

using std::string;
using std::vector;
using ValuePtr = std::shared_ptr<Value>;

int main(int argc, char *argv[]) {
  json constJson = readJson(argv[1]);

  ConstInstr *constInstr = nullptr;
  ValueInstr *valueInstr = nullptr;
  for (auto &brilFcn : constJson.at("functions")) {
    for (auto &brilInstr : brilFcn.at("instrs")) {
      if (brilInstr.at("op").get<string>() == "const") {
        constInstr = new ConstInstr(&brilInstr);

        std::cout << constInstr->getValue()->dump() << std::endl;
      } else if (brilInstr.at("op").get<string>() == "id") {
        ArgValue *constInstrArg = new ArgValue(constInstr);
        valueInstr = new ValueInstr(
            &brilInstr,
            std::make_optional<vector<ArgValue *>>({constInstrArg}));

        std::cout << valueInstr->getValue()->dump() << std::endl;
        delete valueInstr;
        delete constInstr;
        delete constInstrArg;
      }
    }
  }

  return EXIT_SUCCESS;
}