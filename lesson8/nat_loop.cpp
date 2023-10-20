#include "nat_loop.h"
#include <list>

using std::list;
using std::map;
using std::pair;
using std::set;
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
  map<string, bool> visited;
  for (const auto b : cfg.getBasicBlocks())
    visited[b->getLabel()] = false;
  auto allEdges = findAllEdges(entryBlock, visited);

  // Traverse outward from the entry block, remove the edges going forward
  for (const auto b : cfg.getBasicBlocks())
    visited[b->getLabel()] = false;
  auto backEdges = allEdges;

  list<string> queue;

  visited[entryBlock->getLabel()] = true;
  queue.push_back(entryBlock->getLabel());

  while (!queue.empty()) {
    auto s = queue.front();
    queue.pop_front();

    visited[s] = true;

    for (auto succ : cfg.getBlock(s)->getSuccessors()) {
      if (!visited[succ->getLabel()]) {
        queue.push_back(succ->getLabel());
        auto currDirEdge = std::make_pair(s, succ->getLabel());
        if (backEdges.find(currDirEdge) != backEdges.end())
          backEdges.erase(backEdges.find(currDirEdge));
      }
    }
  }

  return backEdges;
}

set<set<string>> findCyclesUtil(shared_ptr<Block> startBlock,
                                shared_ptr<Block> currBlock,
                                map<string, bool> visited) {
  set<set<string>> res;
  if (visited[currBlock->getLabel()]) {
    if (currBlock->getLabel() == startBlock->getLabel()) {
      set<string> currRes;
      for (const auto [k, v] : visited) {
        if (v)
          currRes.insert(k);
      }
      res.insert(currRes);
    }
    return res;
  }

  visited[currBlock->getLabel()] = true;

  for (auto succ : currBlock->getSuccessors()) {
    res = getUnion(res, findCyclesUtil(startBlock, succ, visited));
  }

  visited[currBlock->getLabel()] = false;

  return res;
}

set<set<string>> findCycles(CFG &cfg) {
  set<set<string>> res;

  map<string, bool> visited;
  for (const auto b : cfg.getBasicBlocks())
    visited[b->getLabel()] = false;

  for (const auto b : cfg.getBasicBlocks()) {
    res = getUnion(res, findCyclesUtil(b, b, visited));
  }

  return res;
}

set<set<string>> findNatLoops(CFG &cfg) {
  auto backEdges = findBackEdges(cfg);

  auto allCycles = findCycles(cfg);

  for (const auto &cycle : allCycles) {
    for (const auto &backEdge : backEdges) {
      string tail = backEdge.first;
      string head = backEdge.second;
      if (cycle.find(tail) != cycle.end() && cycle.find(head) != cycle.end()) {
        std::cout << "Both the tail and the head are in the cycle/loop! This "
                     "is potentially a natural loop. And we should eliminate "
                     "it if any of the node, except `head`, has a predecessor "
                     "that is outside this cycle.\n";
      }
    }
  }
}