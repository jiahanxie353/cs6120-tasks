#include "ssa.h"
#include <fstream>

// calculate the number of instructions in a given bril program
int countTotalInstrs(json &brilProg) {
    int res = 0;
    for (const auto &brilFcn : brilProg.at("functions")) {
        if (brilFcn.contains("instrs"))
            res += brilFcn.at("instrs").size();
    }
    return res;
}

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);
    string mode = argv[2];

    std::ostream *os_ptr = &std::cout;
    unique_ptr<std::ofstream> outfile;

    if (mode == "write") {
        string outputJsonName = string(argv[1]);
        size_t dotPos = outputJsonName.rfind('.');
        if (dotPos != string::npos)
            outputJsonName.insert(dotPos + 1, "ssa.");

        outfile = std::make_unique<std::ofstream>(outputJsonName);
        os_ptr = outfile.get();
    }

    json result;
    for (auto &brilFcn : brilProg.at("functions")) {
        json fcnJson = toSSA(brilFcn);

        fcnJson = fromSSA(fcnJson);

        if (brilFcn.contains("args")) {
            json args = brilFcn.at("args");
            fcnJson["args"] = args;
        }

        result["functions"] = json::array();
        result["functions"].push_back(fcnJson);
    }
    *os_ptr << result.dump(4) << std::endl;

    int origInstrsCnt = countTotalInstrs(brilProg);
    int newInstrsCnt = countTotalInstrs(result);
    std::cout << "The percentage of newly added instructions is: "
              << (float)(newInstrsCnt - origInstrsCnt) / (float)origInstrsCnt
              << std::endl;
}