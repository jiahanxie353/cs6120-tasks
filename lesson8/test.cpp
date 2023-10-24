#include "licm.h"
#include "nat_loop.h"

using std::map;
using std::pair;
using std::set;
using std::string;

int main(int argc, char *argv[]) {
  json brilProg = readJson(argv[1]);

  string outputJsonName = string(argv[1]);
  size_t dotPos = outputJsonName.rfind('.');
  if (dotPos != string::npos) {
    string licmStr = "-licm.";
    outputJsonName.insert(dotPos, licmStr.append("json"));
    const int jsonDotLen = 5;
    outputJsonName =
        outputJsonName.substr(0, outputJsonName.size() - jsonDotLen);
  }
  std::ofstream outfile(outputJsonName);

  json outJson;
  outJson["functions"] = json::array();

  for (auto &brilFcn : brilProg.at("functions")) {
    json fcnJson;

    if (brilFcn.contains("name"))
      fcnJson["name"] = brilFcn.at("name");
    if (brilFcn.contains("args"))
      fcnJson["args"] = brilFcn.at("args");

    fcnJson["instrs"] = json::array();

    CFG cfg = CFG(brilFcn);

    set<pair<string, set<string>>> allNatLoops = findNatLoops(cfg);

    std::cout << "Function " << brilFcn.at("name") << " has "
              << allNatLoops.size() << " number of natural loops.\n";

    for (const auto entryLoopPair : allNatLoops) {
      std::cout << "\nThe entry of the current loop is: " << entryLoopPair.first
                << std::endl;
      std::cout << "The corresponding natural loop is: \n";
      for (const auto node : entryLoopPair.second) {
        std::cout << node << std::endl;
      }
      auto preHeader = insertPreheader(cfg, entryLoopPair);

      const string analysis = "reaching definition";
      set<Instr *> initValues;
      if (brilFcn.contains("args")) {
        for (auto &arg : brilFcn.at("args"))
          initValues.insert(&arg);
      }
      DataFlow<Instr *> dataFlow(analysis, cfg, initValues);
      auto reachDef = workList<Instr *>(dataFlow);

      auto lIInstrs = identLoopInvarInstrs(cfg, reachDef, entryLoopPair);

      for (const auto [k, v] : lIInstrs) {
        std::cout << "Block " << k << " has LI: \n";
        for (const auto elm : v) {
          std::cout << elm->dump() << std::endl;
          insertToPreHead(cfg, preHeader, std::make_pair(elm, k),
                          entryLoopPair);
        }
      }

      auto labels = json::array();
      labels.push_back(entryLoopPair.first);
      auto jmpJson = new json({{"labels", labels}, {"op", "jmp"}});
      preHeader->insertInstr(jmpJson, preHeader->getInstrs().size());

      std::cout << "The instructions in the pre-headers are: " << std::endl;
      for (const auto instr : preHeader->getInstrs()) {
        std::cout << instr->dump() << std::endl;
      }
    }

    for (auto block : cfg.getBasicBlocks()) {
      if (block->getSuccessors().size() == 0) {
        auto exitBlock = std::make_shared<Block>("dummy_exit");
        auto labelJson = new json({{"label", "dummy_exit"}});
        exitBlock->insertInstr(labelJson, 0);
        auto retJson = new json({{"op", "ret"}, {"args", json::array()}});
        exitBlock->insertInstr(retJson, 1);
        exitBlock->addPredecessor(block);

        block->addSuccessor(exitBlock);
        auto jmpJson = new json({{"op", "jmp"}, {"labels", {"dummy_exit"}}});
        block->insertInstr(jmpJson, block->getInstrs().size());

        cfg.addBasicBlock(exitBlock);
        break;
      }
    }

    for (const auto block : cfg.getBasicBlocks()) {
      for (const auto instr : block->getInstrs())
        fcnJson["instrs"].push_back(*instr);
    }
    outJson["functions"].push_back(fcnJson);
  }
  outfile << outJson.dump(4) << std::endl;
}