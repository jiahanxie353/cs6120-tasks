#include <assert.h>
#include <stdexcept>

#include "../../block.hpp"
#include "../../cfg.hpp"
#include "../../utils.hpp"

// A recursive function to build all (acyclic) paths from 'u' to 'd'.
// Adapted from
// https://www.geeksforgeeks.org/find-paths-given-source-destination/
void getAllPathsUtil(map<string, vector<string>> &cfg, string u, string d,
                     set<string> &visited, string path[], int &path_index,
                     vector<vector<string>> &allPaths) {
    // Mark the current node and store it in path[]
    visited.insert(u);
    path[path_index] = u;
    path_index++;

    // If current vertex is same as destination, then append current path[]
    if (u == d) {
        vector<string> tmpPath;
        for (int i = 0; i < path_index; i++)
            tmpPath.push_back(path[i]);
        allPaths.push_back(tmpPath);
    } else // If current vertex is not destination
    {
        // Recur for all the vertices adjacent to current vertex
        vector<string>::iterator i;
        for (i = cfg.at(u).begin(); i != cfg.at(u).end(); ++i)
            if (visited.find(*i) == visited.end())
                getAllPathsUtil(cfg, *i, d, visited, path, path_index,
                                allPaths);
    }

    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited.erase(u);
}

vector<vector<string>> getAllPaths(map<string, vector<string>> &cfg, string s,
                                   string d, int numNodes) {
    vector<vector<string>> allPaths;
    // Mark all the vertices as not visited
    set<string> visited{};

    // Create an array to store paths
    string path[numNodes];
    int path_index = 0; // Initialize path[] as empty

    // Call the recursive helper function to get all paths
    getAllPathsUtil(cfg, s, d, visited, path, path_index, allPaths);

    return allPaths;
}

bool isDominator(string dominator, string dominatee, CFG &cfg) {
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

void dominatorTest(string brilFile, string fcnName, string dominator,
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

    assert(isDominator(dominator, dominatee, cfg) == true);
}

int main(int argc, char *argv[]) {
    string brilFile = argv[1];
    string fcnName = argv[2];
    string dominator = argv[3];
    string dominatee = argv[4];
    dominatorTest(brilFile, fcnName, dominator, dominatee);
    json brilProg = readJson(argv[1]);

    // for (auto &brilFcn : brilProg.at("functions")) {
    //     CFG cfg(brilFcn);

    //     auto graph = cfg.getCFG();

    //     cfg.visualize();
    // }

    return EXIT_SUCCESS;
}
