#include "../../block.hpp"

// A recursive function to build all (acyclic) paths from 'u' to 'd'.
// Adapted from
// https://www.geeksforgeeks.org/find-paths-given-source-destination/
void getAllPathsUtil(map<string, vector<string>> &graph, string u, string d,
                     set<string> &visited, string path[], int &path_index,
                     vector<vector<string>> &allPaths);

vector<vector<string>> getAllPaths(map<string, vector<string>> &cfg, string s,
                                   string d, int numNodes);