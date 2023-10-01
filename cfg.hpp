#pragma once

#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "block.hpp"
#include "utils.h"

class CFG {
  public:
    CFG(){};
    // build a cfg out of a json-represented bril function
    CFG(json &brilFcn);
    int getSize() const;
    // return the bril function represented in json
    json &getBrilJson();
    // returns all basic blocks (list of lists of instructions) of this cfg
    vector<shared_ptr<Block>> getBasicBlocks() const;
    set<string> getAllLabels() const;
    // return the block with label name `label` in this cfg
    shared_ptr<Block> getBlock(const string label) const;
    // returns the CFG of this function
    map<string, vector<string>> getCFG() const;
    // get the entry of this cfg
    shared_ptr<Block> getEntry() const;
    // get the exits of this cfg
    vector<shared_ptr<Block>> getExists() const;
    // get the uniform exit sink of this cfg
    shared_ptr<Block> getExitSink() const;
    bool contains(const string) const;
    // visualize the control flow graph using graphviz
    void visualize();
    // get the dominators of a block in this cfg
    set<string> getDominators(string);
    set<string> getStrictDominators(string);
    set<string> getDominatees(string);
    set<string> getStrictDominatees(string);
    set<string> getImmDominatees(string);
    map<string, set<string>> getImmDominateeMap();
    // get all variable names in this cfg
    set<string> getAllVars() const;

    struct domTreeNode {
        domTreeNode(const string strLabel) : label(strLabel) {}
        string label;
        vector<unique_ptr<domTreeNode>> children;
    };
    domTreeNode &getDomTree() const;
    void buildDomTree(const string, const map<string, set<string>>);
    set<string> getDomFrontier(string);
    void printTree(domTreeNode &, int, std::ostream &os = std::cout);
    // find all paths from a block to the target block
    vector<std::deque<string>> findAllPaths(shared_ptr<Block>,
                                            shared_ptr<Block>);
    void insertBtw(shared_ptr<Block> newBlock, shared_ptr<Block> pred,
                   shared_ptr<Block> succ);

  private:
    // raw json representation of this function's cfg
    json rawBrilFcn;
    shared_ptr<Block> entry;
    vector<shared_ptr<Block>> exits;
    shared_ptr<Block> exitSink;
    // indicate whether the cfg has been built or not
    bool built = false;
    // all basic blocks in this cfg
    vector<shared_ptr<Block>> basicBlocks;
    // CFG, a map of label: list of labels/successors
    map<string, vector<string>> cfg;
    map<string, shared_ptr<Block>> label2Block;
    // build basic blocks from all instructions inside a function (each block's
    // label is not yet unassigned)
    vector<shared_ptr<Block>> buildBlocks(vector<Instr *> &);
    // name each block with a corresponding label for this cfg
    void nameBlocks(vector<shared_ptr<Block>>);
    // build CFG from a list of basic block
    map<string, vector<string>> buildCFG(vector<shared_ptr<Block>>);
    // a mapping from a block to all its dominators in this cfg
    map<string, set<string>> dominatorsMap;
    map<string, set<string>> strictDominatorMap;
    // a mapping from a block to all the blocks that it dominate in this cfg
    map<string, set<string>> dominateesMap;
    map<string, set<string>> strictDominateeMap;
    // map from a block to its immediate dominatees
    map<string, set<string>> immDominatees;
    // the set of nodes that are just one edge away from being dominated by a
    // given node
    map<string, set<string>> domFrontier;
    // compute the dominators (and strict dominators along the way) of all
    // blocks in this cfg
    void computeDominators();
    // compute the dominatees (and strict dominatees along the way) of all
    // blocks in this cfg
    void computeDominatees();
    void computeImmDominatees();
    void computeDomFrontier();

    unique_ptr<domTreeNode> tree;
    void populateTree(domTreeNode *, const string,
                      const map<string, set<string>>);
};
