#include "licm.h"

using std::pair;
using std::set;
using std::string;

using reachDefType = tuple<map<shared_ptr<Block>, set<Instr *>>,
                           map<shared_ptr<Block>, set<Instr *>>>;

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

set<string> getAllDefns(set<Instr *> blockInstrs) {
  set<string> res;
  for (const auto instr : blockInstrs) {
    if (instr->contains("dest"))
      res.insert(instr->at("dest").get<string>());
  }
  return res;
}

set<pair<string, Instr *>> findReachedDef(CFG &cfg, string blockLabel,
                                          string argName,
                                          reachDefType reachDef) {
  set<pair<string, Instr *>> res;

  auto ins = std::get<0>(reachDef);
  auto outs = std::get<1>(reachDef);

  auto inIter = ins.begin();
  auto outIter = outs.begin();
  // "use" is either passed through `ins` or is defined inside this block;
  // loop over the `ins` and identify the `in` that defines `use`;
  // for that `in`, loop over the cfg and find the corresponding block
  while (inIter != ins.end()) {
    if (inIter->first->getLabel() == blockLabel) {
      auto allInDefns = getAllDefns(inIter->second);
      auto allOutDefns = getAllDefns(outIter->second);

      // if "use"'s definition is before this block/passed by `ins`
      if (allInDefns.find(argName) != allInDefns.end()) {
        // set<pair<string, Instr *>> res1;
        for (const auto i : inIter->second) {
          if (i->contains("dest") && i->at("dest") == argName) {
            auto reachedInstr = i;
            for (const auto bB : cfg.getBasicBlocks()) {
              for (const auto bBI : bB->getInstrs()) {
                if (bBI == reachedInstr)
                  res.insert(std::make_pair(bB->getLabel(), reachedInstr));
              }
            }
          }
        }
      } else {
        assert(allOutDefns.find(argName) != allOutDefns.end());
        auto block = cfg.getBlock(blockLabel);
        for (vector<Instr *>::reverse_iterator i = block->getInstrs().rbegin();
             i != block->getInstrs().rend(); ++i) {
          if ((*i)->contains("dest") && (*i)->at("dest") == argName) {
            res.insert(std::make_pair(blockLabel, (*i)));
            break;
          }
        }
      }
    }
    ++inIter;
    ++outIter;
  }

  return res;
}

template <class T> bool noInterset(set<T> set1, set<T> set2) {
  bool hasIntersection = false;
  for (const auto &elem : set1) {
    if (set2.find(elem) != set2.end()) {
      hasIntersection = true;
      break;
    }
  }
  return !hasIntersection;
}

set<Instr *> identLoopInvarInstrs(CFG &cfg, reachDefType reachDef,
                                  pair<string, set<string>> natLoop) {
  set<Instr *> isLI;
  // iterate to convergence:
  //  for every instruction in the loop:
  //   mark it as LI iff, for all arguments x, either:
  //    all reaching defintions of x are outside of the loop, or
  //    there's exactly one definition, and it's already marked as LI
  int vLISize = isLI.size();
  do {
    vLISize = isLI.size();
    for (const auto node : natLoop.second) {
      auto block = cfg.getBlock(node);
      std::cout << "This natural loop block is: " << block->getLabel()
                << std::endl;
      for (const auto &instr : block->getInstrs()) {
        if (instr->contains("args")) {
          bool loopInvar = true;
          for (const auto arg : instr->at("args")) {
            string argStr = arg.get<string>();
            auto reachedDefns =
                findReachedDef(cfg, block->getLabel(), arg.get<string>(),
                               reachDef); // for each "use"

            set<string> defnBlocks;
            for (const auto &rD : reachedDefns)
              defnBlocks.insert(rD.first);

            if (!(noInterset<string>(defnBlocks, natLoop.second) ||
                  (defnBlocks.size() == 1 &&
                   isLI.find(reachedDefns.begin()->second) != isLI.end()))) {
              loopInvar = false;
              break;
            }
          }
          if (loopInvar)
            isLI.insert(instr);
        }
      }
    }
  } while (vLISize != isLI.size());

  return isLI;
}