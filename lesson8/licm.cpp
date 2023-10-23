#include "licm.h"

using std::pair;
using std::set;
using std::string;

string genFreshPreHeaderName(CFG &cfg) {
  int currMax = 0;
  for (const auto &bb : cfg.getBasicBlocks()) {
    if (bb->getLabel().substr(0, 9) == "preHeader") {
      int preHeaderNum = std::stoi(bb->getLabel().substr(9));
      int newHeaderNum = ++preHeaderNum;
      if (newHeaderNum > currMax)
        currMax = newHeaderNum;
    }
  }
  return "preHeader" + std::to_string(currMax);
}

void insertPreheader(CFG &cfg, pair<string, set<string>> natLoop) {
  string loopHeader = natLoop.first;

  auto headerBlock = cfg.getBlock(loopHeader);

  string preHeaderName = genFreshPreHeaderName(cfg);

  shared_ptr<Block> preHeader = std::make_shared<Block>(preHeaderName);

  preHeader->addSuccessor(headerBlock);

  for (auto &pred : headerBlock->getPredecessors()) {
    preHeader->addPredecessor(pred);
    pred->addSuccessor(preHeader);

    pred->removeSuccessor(headerBlock);
    headerBlock->removePredecessor(pred);
  }
  headerBlock->addPredecessor(preHeader);

  cfg.addBasicBlock(preHeader);
}