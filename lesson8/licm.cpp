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

shared_ptr<Block> insertPreheader(CFG &cfg, pair<string, set<string>> natLoop) {
  string loopHeader = natLoop.first;

  auto headerBlock = cfg.getBlock(loopHeader);

  string preHeaderName = genFreshPreHeaderName(cfg);

  shared_ptr<Block> preHeader = std::make_shared<Block>(preHeaderName);

  auto jP = new json({{"label", preHeaderName}});
  preHeader->insertInstr(jP, 0);

  preHeader->addSuccessor(headerBlock);

  for (auto &pred : headerBlock->getPredecessors()) {
    if (natLoop.second.find(pred->getLabel()) == natLoop.second.end()) {
      preHeader->addPredecessor(pred);
      pred->addSuccessor(preHeader);

      bool isBranch = false;
      if (pred->getInstrs().back()->contains("op")) {
        if (pred->getInstrs().back()->at("op").get<string>() == "jmp")
          pred->removeInstr(pred->getInstrs().size() - 1);
        if (pred->getInstrs().back()->at("op").get<string>() == "br") {
          isBranch = true;
          for (auto &lbl : pred->getInstrs().back()->at("labels")) {
            if (lbl.get<string>() == headerBlock->getLabel()) {
              lbl = preHeader->getLabel();
            }
          }
        }
      }

      if (!isBranch) {
        auto labels = json::array();
        labels.push_back(preHeaderName);
        auto jP = new json({{"labels", labels}, {"op", "jmp"}});

        pred->insertInstr(jP, pred->getInstrs().size());
      }

      pred->removeSuccessor(headerBlock);
      headerBlock->removePredecessor(pred);
    }
  }
  headerBlock->addPredecessor(preHeader);

  cfg.addBasicBlock(preHeader);

  return preHeader;
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

map<string, set<Instr *>>
identLoopInvarInstrs(CFG &cfg, reachDefType reachDef,
                     pair<string, set<string>> natLoop) {
  map<string, set<Instr *>> res;
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
      set<Instr *> isLI;
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
          if (loopInvar) {
            res[block->getLabel()].insert(instr);
            isLI.insert(instr);
          }
        }
      }
    }
  } while (vLISize != isLI.size());

  return res;
}

bool defnDominateUses(CFG &cfg, pair<Instr *, string> instrBlock,
                      pair<string, set<string>> natLoop) {
  // find the definition of candidateInstr;
  // find in the `natLoop` which blocks uses this definition, if they use, check
  // if `blockLabel` dominates that block
  auto &candidateInstr = instrBlock.first;
  auto &blockLabel = instrBlock.second;

  string definedVar = candidateInstr->at("dest");
  auto dominatees = cfg.getDominatees(blockLabel);

  for (const auto blockName : natLoop.second) {
    auto blockInLoop = cfg.getBlock(blockName);
    for (const auto &instr : blockInLoop->getInstrs()) {
      if (instr->contains("args")) {
        for (const auto &arg : instr->at("args")) {
          if (arg.get<string>() == definedVar) {
            if (dominatees.find(blockName) == dominatees.end())
              return false;
          }
        }
      }
    }
  }
  std::cout << "Definition: " << definedVar << " dominates all uses.\n";
  return true;
}

bool checkReDefn(CFG &cfg, pair<Instr *, string> instrBlock,
                 pair<string, set<string>> natLoop) {
  auto &candidateInstr = instrBlock.first;
  auto &blockLabel = instrBlock.second;

  string definedVar = candidateInstr->at("dest");

  for (const auto blockName : natLoop.second) {
    if (blockName != blockLabel) {
      auto blockInLoop = cfg.getBlock(blockName);
      for (const auto &instr : blockInLoop->getInstrs()) {
        if (instr->contains("dest") && instr->at("dest") == definedVar)
          return false;
      }
    }
  }
  std::cout << "No re-definition of " << definedVar << " in this loop.\n";
  return true;
}

set<string> getAllLoopExits(CFG &cfg, pair<string, set<string>> natLoop) {
  set<string> allExits;

  for (const auto blockLabel : natLoop.second) {
    auto block = cfg.getBlock(blockLabel);
    for (const auto succ : block->getSuccessors()) {
      if (natLoop.second.find(succ->getLabel()) == natLoop.second.end()) {
        allExits.insert(succ->getLabel());
      }
    }
  }
  return allExits;
}

bool dominateAllExits(CFG &cfg, pair<Instr *, string> instrBlock,
                      pair<string, set<string>> natLoop) {
  set<string> allExits = getAllLoopExits(cfg, natLoop);
  std::cout << "Exits: " << std::endl;
  for (const auto exit : allExits)
    std::cout << exit << std::endl;

  auto dominatees = cfg.getDominatees(instrBlock.second);
  for (const auto exit : allExits) {
    if (dominatees.find(exit) == dominatees.end())
      return false;
  }
  return true;
}

void insertToPreHead(CFG &cfg, shared_ptr<Block> preHeader,
                     pair<Instr *, string> instrBlock,
                     pair<string, set<string>> natLoop) {
  // we can move `instr` to `preHeader` iff
  // 1. the definition dominates all uses;
  // 2. no other definitions of the same variable exists in the loop
  // 3. the instruction dominates all loop exits
  auto &candidateInstr = instrBlock.first;

  if (dominateAllExits(cfg, instrBlock, natLoop)) {
    auto block = cfg.getBlock(instrBlock.second);
    if (candidateInstr->contains("op") &&
        (candidateInstr->at("op").get<string>() == "br" ||
         candidateInstr->at("op").get<string>() == "jmp"))
      return;
    if (candidateInstr->contains("dest")) {
      if (defnDominateUses(cfg, instrBlock, natLoop) &&
          checkReDefn(cfg, instrBlock, natLoop)) {
        // save to move
        preHeader->insertInstr(instrBlock.first, preHeader->getInstrs().size());
        block->removeInstr(instrBlock.first);
      }
    } else {
      // save to move
      preHeader->insertInstr(instrBlock.first, preHeader->getInstrs().size());
      block->removeInstr(instrBlock.first);
    }
  }
}