#include "ssa.h"
#include <fstream>

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);
    string mode = argv[2];

    std::ostream *os_ptr = &std::cout;
    unique_ptr<std::ofstream> outfile;

    if (mode == "write") {
        string outputJsonName = string(argv[1]);
        size_t dotPos = outputJsonName.rfind('.');
        if (dotPos != string::npos)
            outputJsonName.insert(dotPos + 1, "phi-nodes.");

        outfile = std::make_unique<std::ofstream>(outputJsonName);
        os_ptr = outfile.get();
    }

    json result;
    for (auto &brilFcn : brilProg.at("functions")) {

        json fcnJson = toSSA(brilFcn);

        fcnJson = fromSSA(fcnJson);

        result["functions"] = json::array();
        result["functions"].push_back(fcnJson);
    }
    *os_ptr << result.dump(4);
}