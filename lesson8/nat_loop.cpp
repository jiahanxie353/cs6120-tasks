#include "nat_loop.h"

using std::pair;
using std::set;
using std::shared_ptr;
using std::string;

template <typename T>
std::set<T> getUnion(const std::set<T> &a, const std::set<T> &b) {
  std::set<T> result = a;
  result.insert(b.begin(), b.end());
  return result;
}

set<pair<string, string>> findAllEdges(shared_ptr<Block> rootBlock,
                                       std::map<std::string, bool> visited) {
  set<pair<string, string>> res;
  visited[rootBlock->getLabel()] = true;
  // The first pass, find all directed edges (=> set<pair>) using successors
  for (const auto succ : rootBlock->getSuccessors()) {
    res.insert(std::make_pair(rootBlock->getLabel(), succ->getLabel()));
    if (!visited[succ->getLabel()])
      res = getUnion(res, findAllEdges(succ, visited));
  }
  return res;
}

set<pair<string, string>> findBackEdges(CFG &cfg) {
  // The first pass, find all directed edges (=> set<pair>) using successors
  auto entryBlock = cfg.getEntry();
  // need to bfs
}

set<set<string>> findNatLoops(CFG &cfg) {}