#pragma once
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <type_traits>
#include <vector>

#include "../block.hpp"
#include "../cfg.hpp"
#include "../utils.h"

const string reachDef = "reaching definition";

namespace dataflow {
template <class T>
using mergeType = std::function<set<T>(const vector<set<T>> &)>;
template <class T>
using transferType =
    std::function<set<T>(const shared_ptr<Block>, const set<T> &)>;
} // namespace dataflow

// a data flow framework
template <class T> class DataFlow {
public:
  DataFlow(const string &a, CFG &c, const set<T> &iv)
      : analysis(a), cfg(c), initValue(iv) {
    tuple<dataflow::mergeType<T>, dataflow::transferType<T>> mergeTransfer =
        mergeTransferFactory(a);
    merge = std::get<0>(mergeTransfer);
    transfer = std::get<1>(mergeTransfer);
  }
  CFG &getCFG() const { return cfg; }
  set<T> getInitValue() const { return initValue; }
  // merge a list of predecessors blocks' values
  dataflow::mergeType<T> merge;
  // transfer a set of values passed as inputs and a list of values inside the
  // current block, and return the transferred results as the outputs
  dataflow::transferType<T> transfer;

private:
  // the analysis name
  string analysis;
  // the input cfg
  CFG &cfg;
  // the initial values passed to the data flow framework
  set<T> initValue;

  tuple<dataflow::mergeType<T>, dataflow::transferType<T>>
  mergeTransferFactory(const string analysis) {
    if (analysis.compare(reachDef) == 0) {
      static_assert(std::is_same<T, Instr *>::value, "T must be Instruction");

      dataflow::mergeType<T> merge =
          [this](const vector<set<T>> &outPreds) -> set<T> {
        // union sets of strings
        set<T> unionedRes;
        for (const auto &s : outPreds)
          unionedRes.insert(s.begin(), s.end());
        return unionedRes;
      };

      dataflow::transferType<T> transfer =
          [this](const shared_ptr<Block> currBlock,
                 const set<T> &inputSet) -> set<T> {
        set<T> defined = currBlock->getDefined<T>();
        set<T> killed = currBlock->computeKilled<T>(inputSet);
        set<T> difference;
        std::set_difference(inputSet.begin(), inputSet.end(), killed.begin(),
                            killed.end(),
                            std::inserter(difference, difference.begin()));
        set<T> unionedRes;
        std::set_union(defined.begin(), defined.end(), difference.begin(),
                       difference.end(),
                       std::inserter(unionedRes, unionedRes.begin()));
        return unionedRes;
      };

      return std::make_tuple(merge, transfer);
    }
    throw std::runtime_error("Data flow analysis not supported: " + analysis);
  }
};

// the worklist algorithm for solving a data flow problem
template <class T>
tuple<map<shared_ptr<Block>, set<T>>, map<shared_ptr<Block>, set<T>>>
workList(DataFlow<T> &df) {
  CFG &cfg = df.getCFG();
  shared_ptr<Block> entry = cfg.getEntry();

  map<shared_ptr<Block>, set<T>> in, out;
  in.insert({entry, df.getInitValue()});
  for (const auto block : cfg.getBasicBlocks()) {
    out.insert({block, df.getInitValue()});
  }

  std::queue<shared_ptr<Block>> worklist;
  for (const auto b : cfg.getBasicBlocks())
    worklist.push(b);
  while (!worklist.empty()) {
    shared_ptr<Block> block = worklist.front();
    worklist.pop();

    if (block->getPredecessors().size() > 0) {
      vector<set<T>> outPreds;
      for (const auto pred : block->getPredecessors())
        outPreds.push_back(out[pred]);

      set<T> predsOut = df.merge(outPreds);
      in[block] = predsOut;
    }

    auto currOut = out[block];
    set<T> transferRes = df.transfer(block, in[block]);
    out[block] = df.transfer(block, in[block]);

    if (currOut != out[block]) {
      for (const auto succ : block->getSuccessors())
        worklist.emplace(succ);
    }
  }
  return std::make_tuple(in, out);
}
