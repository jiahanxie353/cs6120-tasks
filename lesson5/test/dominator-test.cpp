#include "../../block.hpp"
#include "../../cfg.hpp"
#include "../../utils.hpp"

// Adapted from
// https://www.geeksforgeeks.org/count-possible-paths-two-vertices/#
/*
    Counts the number of paths from the source vertex/block to
    the destination vertex/block in the cfg.
*/
int countPaths(const map<string, vector<string>> &cfg, string srcLabel,
               string destLabel, set<string> &visited) {
    if (srcLabel == destLabel) {
        return 1;
    }

    if (visited.find(srcLabel) != visited.end()) {
        return 0;
    }

    int count = 0;
    visited.insert(srcLabel);

    // Recursively count the number of paths from each of
    // the source vertex's neighbors to the destination
    // vertex.
    for (string neighbor : cfg.at(srcLabel)) {
        std::cout << neighbor << std::endl;
        if (visited.find(neighbor) == visited.end()) {
            count += countPaths(cfg, neighbor, destLabel, visited);
        }
    }

    visited.erase(srcLabel);

    return count;
}

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);

    for (auto &brilFcn : brilProg.at("functions")) {
        CFG cfg(brilFcn);

        auto graph = cfg.getCFG();

        auto basicBlocks = cfg.getBasicBlocks();

        string src = basicBlocks[0]->getLabel();
        string dest = basicBlocks.back()->getLabel();
        set<string> visited{};

        std::cout << src << std::endl;
        std::cout << countPaths(graph, src, dest, visited) << std::endl;
    }

    return EXIT_SUCCESS;
}
