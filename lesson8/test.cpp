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

    std::set<std::set<std::string>> allCycles = findCycles(cfg);

    for (const auto cycle : allCycles) {
      std::cout << "A loop: \n";
      for (const auto node : cycle) {
        std::cout << node << std::endl;
      }
    }
  }
}