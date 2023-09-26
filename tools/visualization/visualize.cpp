#include "../../cfg.hpp"
#include "../../utils.hpp"

int main(int argc, char *argv[]) {
    json brilProg = readJson(argv[1]);
    for (auto &brilFcn : brilProg.at("functions")) {
        CFG cfg(brilFcn);
        cfg.visualize();
    }

    return EXIT_SUCCESS;
}