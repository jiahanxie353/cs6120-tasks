#include "data_flow.hpp"

#include <algorithm>
#include <queue>
#include <type_traits>

const string Phi = "\u03A6";

int main(int argc, char *argv[]) {
  const string analysis = reachDef;
  json brilProg = readJson(argv[1]);
  for (auto &brilFcn : brilProg.at("functions")) {
    std::cout << brilFcn.at("name").dump() << ":" << std ::endl;
    set<Instr *> initValues;
    if (brilFcn.contains("args")) {
      for (auto &arg : brilFcn.at("args"))
        initValues.insert(&arg);
    }
    CFG cfg(brilFcn);

    DataFlow<Instr *> dataFlow(analysis, cfg, initValues);
    auto [in, out] = workList<Instr *>(dataFlow);

    auto itIn = in.begin();
    auto itOut = out.begin();

    while (itIn != in.end()) {
      std::cout << "    " << itIn->first->getLabel() << ":" << std::endl;

      std::cout << "        "
                << "in: ";
      if (itIn->second.size() == 0)
        std::cout << Phi;
      else {
        for (auto setIt = itIn->second.begin(); setIt != itIn->second.end();
             ++setIt) {
          // std::cout << (*setIt)->at("dest").get<string>();
          std::cout << (*setIt)->dump();
          if (std::next(setIt) != itIn->second.end())
            std::cout << ", ";
        }
      }
      std::cout << std::endl;

      std::cout << "        "
                << "out: ";
      if (itOut->second.size() == 0)
        std::cout << Phi;
      else {
        for (auto setIt = itOut->second.begin(); setIt != itOut->second.end();
             ++setIt) {
          std::cout << (*setIt)->dump();
          if (std::next(setIt) != itOut->second.end())
            std::cout << ", ";
        }
      }
      std::cout << std::endl;
      ++itIn;
      ++itOut;
    }
  }

  return EXIT_SUCCESS;
}