#include "ssa.h"

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);

    for (auto &brilFcn : brilProg.at("functions")) {
        CFG cfg(brilFcn);

        insertPhiNodes(cfg);
    }
}