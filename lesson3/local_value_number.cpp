#include "local_value_number.hpp"

// mapping from expression tuples to their value numbering
std::map<ExprTuple, int> exprNumTbl;
// mapping from variable names to their current value numbering row number in
// the table
std::map<Var, int> var2Num;
// map from value numbering to canonical variables; there can be multiple
// canonical homes, always retrieve the first one
std::map<int, std::vector<Var>> canonHome;
// map from value numbering to const values/str literals
std::map<int, std::string> num2Const;
// the freshest numbering
int numbering;

ExprTuple genExprTuple(Instr*);
void insertExprNumBinding(ExprTuple, std::string);

json lvn(json brilProg) {
    std::tuple<std::vector<Block>, std::vector<std::vector<bool>>,
               std::vector<std::set<Var>>>
        blocksOverwrites = genBlocksOverwrites(brilProg);
    // list of blocks in the given bril program
    std::vector<Block> allBlocks = std::get<0>(blocksOverwrites);
    // bool to indicate whether each instruction's dest will be overwritten in
    // its local block (effect operation instructions are assumed never to be
    // overwritten)
    std::vector<std::vector<bool>> overwrites = std::get<1>(blocksOverwrites);
    std::vector<std::set<Var>> allVarNames = std::get<2>(blocksOverwrites);

    for (size_t blockIdx = 0; blockIdx < allBlocks.size(); ++blockIdx) {
        // it's local analysis, so everything is per-block
        numbering = 0;
        exprNumTbl.clear();
        var2Num.clear();
        canonHome.clear();

        Block block = allBlocks[blockIdx];
        std::set<Var> blockVarNames = allVarNames[blockIdx];
        for (size_t instrIdx = 0; instrIdx < block.size(); ++instrIdx) {
            Instr* instr = block[instrIdx];
            // effect operations will not produce values, so we don't care about
            // replacing any value
            if (!instr->contains("dest")) {
                continue;
            }

            ExprTuple expr = genExprTuple(instr);

            int num;
            if (exprNumTbl.find(expr) != exprNumTbl.end()) {
                // the value has already been computed, we retrieve its
                // numbering and will reuse it
                num = exprNumTbl[expr];
                canonHome[num].push_back(instr->at("dest"));
                // we keep track of all possible variables that have the same
                // value numbering, but we only use the first one
                Var cHome = canonHome[num][0];

                // replace the old instruction with instr.dest = id
                // existCanonHome
                json tmpId = R"( {"op": "id"} )"_json;
                json tmpArg;
                tmpArg["args"] = json::array({cHome});
                instr->update(tmpId);
                instr->update(tmpArg);
            } else {
                numbering += 1;
                num = numbering;
                std::string dest = instr->at("dest").get<std::string>();
                if (overwrites[blockIdx][instrIdx]) {
                    Var freshName = dest;
                    while (blockVarNames.find(freshName) !=
                           blockVarNames.end()) {
                        freshName += dest;
                    }
                    blockVarNames.insert(freshName);
                    dest = freshName;
                    instr->operator[]("dest") = dest;
                }
                insertExprNumBinding(expr, dest);

                json newArgs = json::array();
                if (instr->operator[]("op") != "const") {
                    for (const auto arg : instr->at("args")) {
                        int argNum = var2Num[arg.get<std::string>()];
                        std::string argNumCHome = canonHome[argNum][0];
                        newArgs.push_back(argNumCHome);
                    }
                    instr->operator[]("args") = newArgs;
                }
            }
            Var dest = instr->at("dest").get<std::string>();
            auto iter = var2Num.find(dest);
            if (iter != var2Num.end())
                iter->second = num;
            else {
                var2Num.insert({dest, num});
            }
        }
    }

    return brilProg;
}

/* Generates an expression tuple (op, [num_i, num_j]) from an instruction. If op
 * is const, returns (const, [numbering_of_const_value])
 */
ExprTuple genExprTuple(Instr* instr) {
    Op op = instr->at("op");
    std::vector<int> nums;
    if (op == "const") {
        const std::string constVal = instr->at("value").dump();

        bool constExist = false;
        for (const auto& it : num2Const) {
            if (it.second == constVal) {
                nums = {it.first};
                constExist = true;
                break;
            }
        }

        if (!constExist) {
            nums = {numbering + 1};
            num2Const.insert({numbering + 1, constVal});
        }
    } else {
        if (instr->contains("args")) {
            for (const auto& arg : instr->at("args")) {
                // arg names `arg`s (excluding const str literals) are
                // guaranteed to exist in var2Num, otw the program will raise an
                // error during semantic analysis
                nums.push_back(var2Num[arg]);
            }
        }
    }

    return std::make_tuple(op, nums);
}

/* Insert a new {expression tuple: numbering}; insert a new canonical home to
 * numberings's canonHome, insert "#{numbering}" if that variable will be
 * overwritten
 */
void insertExprNumBinding(ExprTuple expr, std::string dest) {
    exprNumTbl.insert({expr, numbering});
    for (auto& mapIter : canonHome) {
        std::vector<std::string>& canonVars = mapIter.second;
        for (auto vectIter = canonVars.begin(); vectIter != canonVars.end();
             ++vectIter) {
            if (*vectIter == dest) {
                if (canonVars.size() <= 1) {
                    int curVarNum = var2Num[canonVars[0]];
                    canonVars[0] = "#" + std::to_string(curVarNum);
                } else {
                    vectIter = canonVars.erase(vectIter);
                }
                break;
            }
        }
    }
    std::vector<Var> newCHome = {dest};
    canonHome.insert({numbering, newCHome});
}
