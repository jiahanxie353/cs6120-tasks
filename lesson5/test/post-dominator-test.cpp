#include <assert.h>
#include <stdexcept>

#include "../../block.hpp"
#include "../../cfg.hpp"
#include "../../utils.hpp"
#include "./utils.h"

bool isPostDominator(string dominator, string dominatee, CFG &cfg) {
    string exit = cfg.getExitSink()->getLabel();

    string src = dominatee;
    string dest = exit;

    map<string, vector<string>> graph = cfg.getCFG();

    vector<vector<string>> allPaths =
        getAllPaths(graph, src, dest, cfg.getSize());

    for (const auto path : allPaths) {
        if (std::find(path.begin(), path.end(), dominator) == path.end())
            return false;
    }

    return true;
}

void postDominatorTest(string brilFile, string fcnName, string dominator,
                       string dominatee) {
    json brilProg = readJson(brilFile);

    bool foundFcn = false;
    bool foundDR = false;
    bool foundDE = false;

    CFG cfg;
    for (auto &brilFcn : brilProg.at("functions")) {
        if (foundFcn && foundDR && foundDE)
            break;

        if (brilFcn.at("name") == fcnName) {
            foundFcn = true;

            cfg = CFG(brilFcn);
            auto basicBlocks = cfg.getBasicBlocks();

            for (const auto block : basicBlocks) {
                if (block->getLabel() == dominator)
                    foundDR = true;
                if (block->getLabel() == dominatee)
                    foundDE = true;
            }
        }
    }

    if (!foundFcn)
        throw std::runtime_error("Function not found!");
    if (!foundDR)
        throw std::runtime_error("Dominator block not found!");
    if (!foundDE)
        throw std::runtime_error("Dominatee block not found!");

    assert(isPostDominator(dominator, dominatee, cfg) == true);
}

int main(int argc, char *argv[]) {
    string brilFile = argv[1];
    string fcnName = argv[2];
    string dominator = argv[3];
    string dominatee = argv[4];

    postDominatorTest(brilFile, fcnName, dominator, dominatee);

    return EXIT_SUCCESS;
}
