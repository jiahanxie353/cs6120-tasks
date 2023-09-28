#pragma once
#include <iostream>
#include <set>
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
    DataFlow(const string &a, CFG &c, const set<T> &iv);
    CFG &getCFG() const;
    set<T> getInitValue() const;
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
    mergeTransferFactory(const string analysis);
};

// the worklist algorithm for solving a data flow problem
template <class T>
tuple<map<shared_ptr<Block>, set<T>>, map<shared_ptr<Block>, set<T>>>
workList(DataFlow<T> &);
