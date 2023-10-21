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

    std::set<std::set<std::string>> allNatLoops = findNatLoops(cfg);

    for (const auto cycle : allNatLoops) {
      std::cout << "A natural loop: \n";
      for (const auto node : cycle) {
        std::cout << node << std::endl;
      }
    }
  }
}