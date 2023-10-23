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
      insertPreheader(cfg, entryLoopPair);

      for (const auto b : cfg.getBasicBlocks()) {
        std::cout << b->getLabel() << "'s predecessors are: \n";
        for (const auto p : b->getPredecessors()) {
          std::cout << p->getLabel() << std::endl;
        }
        std::cout << b->getLabel() << "'s successors are: \n";
        for (const auto s : b->getSuccessors()) {
          std::cout << s->getLabel() << std::endl;
        }
      }
    }
  }
}