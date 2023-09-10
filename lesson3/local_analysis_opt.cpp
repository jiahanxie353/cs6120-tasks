#include <iostream>

#include "global_unused_instrs.hpp"
#include "local_kill_instrs.hpp"
#include "local_value_number.hpp"

int main(int argc, char* argv[]) {
    // std::ofstream outfile = genOutFile(argv[1], "_lvn");

    json brilProg = readJson(argv[1]);

    json firstPass = lvn(brilProg);
    json secondPass = local_kill(firstPass);
    json thirdPass = global_unused(secondPass);

    std::cout << thirdPass.dump(4);

    return EXIT_SUCCESS;
}