#include <assert.h>
#include <iostream>

#include "../../instr.h"
#include "../../utils.h"

int main(int argc, char *argv[]) {
  json constJson = readJson(argv[1]);

  for (auto &brilFcn : constJson.at("functions")) {
    for (auto &brilInstr : brilFcn.at("instrs")) {
      ConstInstr *constInstr = new ConstInstr(&brilInstr);

      std::cout << constInstr->getValue()->dump() << std::endl;

      delete constInstr;
    }
  }

  return EXIT_SUCCESS;
}