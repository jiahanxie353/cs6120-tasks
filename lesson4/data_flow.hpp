#pragma once
#include <iostream>
#include <set>
#include <vector>

#include "../block.hpp"
#include "../cfg.hpp"
#include "../utils.hpp"

// a data flow framework
template <class T>
class DataFlow {
   public:
    DataFlow(const string a, CFG c, set<T> iv)
        : analysis(a), cfg(c), initValue(iv){};

   private:
    // the analysis name
    const string analysis;
    // the input cfg
    CFG cfg;
    // the initial values passed to the data flow framework
    set<T> initValue;
    // merge a list of predecessors blocks' values
    std::function<set<T>&(const vector<set<T>>&)> merge;
    // transfer a set of values passed as inputs and a list of values inside the
    // current block, and return the transferred results as the outputs
    std::function<set<T>&(const set<T>&, const vector<T>&)> transfer;

    void mergeTransferFactory(const string analysis) {
        if (analysis == "reaching definition") {
            auto merge = [](const vector<set<T>>& sets) -> set<T>& {
                // union sets of strings
                set<T> result;
                for (const auto& s : sets) {
                    result.insert(s.begin(), s.end());
                }
                return result;
            };

            auto transfer = [](const set<T>& inputSet,
                               const vector<T>& values) -> set<T>& {};
        }
    }
};

// the worklist algorithm for solving a data flow problem
template <class T>
void workList(DataFlow<T>&);
