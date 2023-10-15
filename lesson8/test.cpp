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

    entryBlock = cfg.getEntry();

    map<string, bool> visited;
    for (const auto b : cfg.getBasicBlocks())
      visited[b->getLabel()] = false;

    set<pair<string, string>> allEdges = findAllEdges(entryBlock, visited);

    for (const auto e : allEdges) {
      std::cout << e.first << ", " << e.second << std::endl;
    }
  }
}