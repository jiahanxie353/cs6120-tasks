#include "licm.h"
#include "nat_loop.h"

using std::map;
using std::pair;
using std::set;
using std::string;

int main(int argc, char *argv[]) {
  json brilProg = readJson(argv[1]);

  std::shared_ptr<Block> entryBlock;
  for (auto &brilFcn : brilProg.at("functions")) {
    CFG cfg = CFG(brilFcn);

    std::set<std::pair<std::string, std::set<std::string>>> allNatLoops =
        findNatLoops(cfg);

    for (const auto entryLoopPair : allNatLoops) {
      std::cout << "The entry of the current loop is: " << entryLoopPair.first
                << std::endl;
      std::cout << "The corresponding natural loop is: \n";
      for (const auto node : entryLoopPair.second) {
        std::cout << node << std::endl;
      }
      auto preHeader = insertPreheader(cfg, entryLoopPair);

      const string analysis = "reaching definition";
      set<Instr *> initValues;
      if (brilFcn.contains("args")) {
        for (auto &arg : brilFcn.at("args"))
          initValues.insert(&arg);
      }
      DataFlow<Instr *> dataFlow(analysis, cfg, initValues);
      auto reachDef = workList<Instr *>(dataFlow);

      auto lIInstrs = identLoopInvarInstrs(cfg, reachDef, entryLoopPair);

      for (const auto [k, v] : lIInstrs) {
        std::cout << "Block " << k << " has LI: \n";
        for (const auto elm : v) {
          std::cout << elm->dump() << std::endl;
          insertToPreHead(cfg, preHeader, std::make_pair(elm, k),
                          entryLoopPair);
        }
      }

      std::cout << "The instructions in the pre-headers are: " << std::endl;
      for (const auto instr : preHeader->getInstrs()) {
        std::cout << instr->dump() << std::endl;
      }
    }
  }
}