#include "ssa.h"
#include <fstream>

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);

    string outputJsonName = string(argv[1]);
    size_t dotPos = outputJsonName.rfind('.');
    if (dotPos != string::npos)
        outputJsonName.insert(dotPos + 1, "phi-nodes.");

    std::ofstream outfile(outputJsonName);

    json result;
    for (auto &brilFcn : brilProg.at("functions")) {
        CFG cfg(brilFcn);

        insertPhiNodes(cfg);

        renameVars(cfg);

        json fcnJson;
        result["functions"] = json::array();
        fcnJson["name"] = "main";
        fcnJson["instrs"] = json::array();
        for (const auto &block : cfg.getBasicBlocks()) {
            for (const auto &instr : block->getInstrs()) {
                fcnJson["instrs"].push_back(*instr);
            }
        }
        result["functions"].push_back(fcnJson);
    }
    outfile << result.dump(4);
}