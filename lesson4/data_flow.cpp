#include "data_flow.hpp"

#include <queue>

template <class T>
set<T>& mergeSets(const vector<set<T>>& sets) {
    set<T>& result;
    for (const auto& s : sets) result.insert(s.begin(), s.end());
    return result;
}

map<string, vector<Var>> reachDefn(
    CFG cfg, vector<string> initVals,
    std::function<set<int>&(const vector<set<int>>&)> setsUnion) {}

int main(int argc, char* argv[]) {
    const string analysis = "reaching definition";
    json brilProg = readJson(argv[1]);
    for (auto& brilFcn : brilProg.at("functions")) {
        set<string> initValues;
        if (brilFcn.contains("args")) {
            for (const auto& arg : brilFcn.at("args"))
                initValues.insert(arg.get<string>());
        }
        CFG cfg(brilFcn);
        // DataFlow<string> dataFlow(analysis, cfg, initValues);
    }

    return EXIT_SUCCESS;
}

template <class T>
void workList(DataFlow<T>& df) {
    CFG cfg = df.cfg;
    Block* entry = cfg.getEntry();
    map<Block*, set<T>> in, out;
    in.insert(entry, df.initValue);
    for (const auto block : cfg.getBasicBlocks()) {
        out.insert({block, df.initValue});
    }
    // instantiate in & out according to df.cfg.allBlocks' size(), where in,
    // out = [map{block_label: set of values}]

    // worklist is a FIFO queue of all blocks
    // while worklist is not empty
    //  block = worklist.pop()
    //  in[block.label] = df.merge(out[p.label] for p in block.predecessors)
    //  out[block.label] = transfer()
}