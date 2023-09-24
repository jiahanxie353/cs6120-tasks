#include <assert.h>
#include <stdexcept>

#include "../../block.hpp"
#include "../../cfg.hpp"
#include "../../utils.hpp"
#include "./utils.h"

using domTreeNode = CFG::domTreeNode;

bool isDominator(string dominator, string dominatee, CFG &cfg) {
    string entry = cfg.getEntry()->getLabel();

    string dest = dominatee;

    map<string, vector<string>> graph = cfg.getCFG();

    vector<vector<string>> allPaths =
        getAllPaths(graph, entry, dest, cfg.getSize());

    for (const auto path : allPaths) {
        if (std::find(path.begin(), path.end(), dominator) == path.end())
            return false;
    }

    return true;
}

void dominatorTest(string brilFile, string fcnName, string dominatee) {
    json brilProg = readJson(brilFile);

    bool foundFcn = false;
    bool foundDE = false;

    CFG cfg;
    for (auto &brilFcn : brilProg.at("functions")) {
        if (foundFcn && foundDE)
            break;

        if (brilFcn.at("name") == fcnName) {
            foundFcn = true;

            cfg = CFG(brilFcn);
            auto basicBlocks = cfg.getBasicBlocks();

            for (const auto block : basicBlocks) {
                if (block->getLabel() == dominatee)
                    foundDE = true;
            }
        }
    }

    if (!foundFcn)
        throw std::runtime_error("Function not found!");
    if (!foundDE)
        throw std::runtime_error("Dominatee block not found!");

    cfg.computeDominators();
    cfg.computeDominatees();
    cfg.computeImmDominatees();
    cfg.buildDomTree(cfg.getEntry()->getLabel(), cfg.getImmDominateeMap());
    cfg.printTree(cfg.getDomTree(), 0);

    for (const auto dominator : cfg.getDominators(dominatee))
        assert(isDominator(dominator, dominatee, cfg) == true);
}

int main(int argc, char *argv[]) {
    string brilFile = argv[1];
    string fcnName = argv[2];
    string dominatee = argv[3];

    dominatorTest(brilFile, fcnName, dominatee);

    return EXIT_SUCCESS;
}
