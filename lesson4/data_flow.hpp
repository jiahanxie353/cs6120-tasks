#pragma once
#include <iostream>
#include <set>
#include <vector>

#include "../cfg.hpp"
#include "../utils.hpp"

// a data flow framework
template <class T>
class DataFlow {
   public:
    DataFlow(string a, CFG c, set<T> iv,
             std::function<set<T>(vector<set<T>>)> m,
             std::function<set<T>(set<T>, vector<T>)> t)
        : analysis(a), cfg(c), initValue(iv), merge(m), transfer(t){};
    // merge a list of predecessors blocks' values
    std::function<set<T>(vector<set<T>>)> merge;
    // transfer a set of values passed as inputs and a list of values inside the
    // current block, and return the transferred results as the outputs
    std::function<set<T>(set<T>, vector<T>)> transfer;

   private:
    // the analysis name
    string analysis;
    // the input cfg
    CFG cfg;
    // the initial values passed to the data flow framework
    set<T> initValue;
};

// the worklist algorithm for solving a data flow problem
template <class T>
void workList(DataFlow<T>&);