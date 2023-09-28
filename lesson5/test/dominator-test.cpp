#include <assert.h>
#include <stdexcept>

#include "../../block.hpp"
#include "../../cfg.hpp"
#include "../../utils.h"
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

    for (const auto &frontier : cfg.getDomFrontier("block5"))
        std::cout << frontier << std::endl;
    for (const auto dominator : cfg.getDominators(dominatee))
        assert(isDominator(dominator, dominatee, cfg) == true);
}

void domTreeTest(string brilFile, string fcnName, string os) {
    json brilProg = readJson(brilFile);
    for (auto &brilFcn : brilProg.at("functions")) {
        if (brilFcn.at("name") == fcnName) {
            CFG cfg(brilFcn);
            cfg.buildDomTree(cfg.getEntry()->getLabel(),
                             cfg.getImmDominateeMap());

            if (os != "cout") {
                string outputJsonName = string(brilFile);
                size_t dotPos = outputJsonName.rfind('.');
                if (dotPos != string::npos) {
                    string treeStr = "tree.";
                    outputJsonName.insert(dotPos + 1, treeStr.append(os));
                    outputJsonName =
                        outputJsonName.substr(0, outputJsonName.size() - 4);
                }
                std::ofstream outfile(outputJsonName);
                cfg.printTree(cfg.getDomTree(), 0, outfile);
            } else {
                cfg.printTree(cfg.getDomTree(), 0);
            }

            return;
        }
    }
    throw std::runtime_error("Function not found!");
}

int main(int argc, char *argv[]) {
    assert(argc >= 4);

    string testSwitch = argv[1];
    string brilFile = argv[2];
    string fcnName = argv[3];

    if (testSwitch == "tree") {
        string os = argv[4];
        domTreeTest(brilFile, fcnName, os);
    } else if (testSwitch == "frontier") {
        string node = argv[4];
    } else {
        throw std::runtime_error("Invalid test type!\n");
    }

    return EXIT_SUCCESS;
}
