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

    set<pair<string, string>> backEdges = findBackEdges(cfg);

    for (const auto e : backEdges) {
      std::cout << e.first << ", " << e.second << std::endl;
    }
  }
}