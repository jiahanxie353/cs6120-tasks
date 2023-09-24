#include "./utils.h"

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