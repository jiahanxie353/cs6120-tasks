#include "data_flow.hpp"

#include <queue>

// TODO: refactor Block,and CFG first
int main(int argc, char* argv[]) {
    json brilProg = readJson(argv[1]);
    for (auto& brilFcn : brilProg.at("functions")) {
        CFG cfg(brilFcn);
    }

    return EXIT_SUCCESS;
}

template <class T>
void workList(DataFlow<T>& df) {
    // instantiate in & out according to df.cfg.allBlocks' size(), where in,
    // out = [map{block_label: set of values}]

    // worklist is a FIFO queue of all blocks
    // while worklist is not empty
    //  block = worklist.pop()
    //  in[block.label] = df.merge(out[p.label] for p in block.predecessors)
    //  out[block.label] = transfer()
}