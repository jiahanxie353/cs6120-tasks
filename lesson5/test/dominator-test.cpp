#include <list>

#include "../../block.hpp"
#include "../../cfg.hpp"
#include "../../utils.hpp"

// A recursive function to build all (acyclic) paths from 'u' to 'd'.
// Adapted from
// https://www.geeksforgeeks.org/find-paths-given-source-destination/
vector<vector<string>> allPaths;

void getAllPathsUtil(map<string, vector<string>> &cfg, string u, string d,
                     set<string> &visited, string path[], int &path_index) {
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
                getAllPathsUtil(cfg, *i, d, visited, path, path_index);
    }

    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited.erase(u);
}

void getAllPaths(map<string, vector<string>> &cfg, string s, string d,
                 int numNodes) {
    // Mark all the vertices as not visited
    set<string> visited{};

    // Create an array to store paths
    string path[numNodes];
    int path_index = 0; // Initialize path[] as empty

    // Call the recursive helper function to get all paths
    getAllPathsUtil(cfg, s, d, visited, path, path_index);
}

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);

    for (auto &brilFcn : brilProg.at("functions")) {
        CFG cfg(brilFcn);

        auto graph = cfg.getCFG();

        auto basicBlocks = cfg.getBasicBlocks();

        string src = basicBlocks[0]->getLabel();
        string dest = basicBlocks.back()->getLabel();

        getAllPaths(graph, src, dest, cfg.getSize());

        // for (const auto path : allPaths) {
        //     for (const auto node : path)
        //         std::cout << node << " ";
        //     std::cout << std::endl;
        // }

        cfg.visualize();
    }

    return EXIT_SUCCESS;
}
