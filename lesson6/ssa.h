#include "../block.hpp"
#include "../cfg.hpp"

void insertPhiNodes(CFG &);

void renameVars(CFG &);

// convert a bril function to its SSA form
json &toSSA(json &brilFcn);

// convert out of `ssaForm` to generate code for real machines
json &fromSSA(json &ssaForm);