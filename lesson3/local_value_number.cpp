#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "cfg.hpp"
#include "utils.hpp"

bool DEBUG = false;

using Args = std::vector<Var>;
using ExprTuple = std::tuple<Op, std::vector<int>>;

std::map<ExprTuple, int>
    exprNumTbl;  // mapping from expression tuples to their value numbering
std::map<Var, int> var2Num;  // mapping from variable names to their current
                             // value numbering row number in the table
std::map<int, std::vector<Var>>
    canonHome;  // map from value numbering to canonical variables; there can be
                // multiple canonical homes, always retrieve the first one
std::map<int, std::string> num2Const;
/**  variable substitution during on-the-fly optimization: (e.g., when you have
 * already computed `a + b` and now you encounter this expression again, first
 * use var2Num to find the numbering of `a` and `b`; then create the expression
 * tuple `(add, #1, #2)`; then use `LVNTable.get(exprTuple)` to find its
 * canonical home `canonVar`; finally, substitute the original expression by (1)
 * `id` operation, if `exprTuple` already has a canonical home; (2)`op,
 * [canonHome.get(arg) for arg in args]`
 * Workflow: for curVar in curExpr => var2Num(curValue) to get value numbering
 * => canonHome(valNumber) to get its canonical home => substitute every
 * variable in the current by the canonical variable => have a new expression =>
 * check if the new expression is already computed, i.e., in the LVNTable => if
 * yes, use `id` operation; otw, use the new expression with the same op, but
 * with original variables substituted by the canonical ones.**/
int numbering;

ExprTuple genExprTuple(Instr*);  // generates an expression tuple (op, num_i,
                                 // num_j); special case when op = const
int insertExprNumBinding(ExprTuple,
                         Instr*);  // auto increment `numbering` by one, and
                                   // insert a new {expression tuple: numbering}
                                   // binding to valNumTblvar2Num.insert()
void updateVar2Num(Var, int);
void updateExpr(Instr*, Var);

int main(int argc, char* argv[]) {
    std::ofstream outfile = genOutFile(argv[1], "_lvn");

    json brilProg = readJson(argv[1]);

    std::vector<Block> allBlocks = genAllBlocks(brilProg);
    for (auto& block : allBlocks) {
        // it's local analysis, so everything is per-block
        numbering = 0;
        exprNumTbl.clear();
        var2Num.clear();
        canonHome.clear();
        for (auto instr : block) {
            if (!instr->contains("dest")) {
                continue;
            }
            ExprTuple expr = genExprTuple(instr);

            int num;
            if (exprNumTbl.find(expr) != exprNumTbl.end()) {
                num = exprNumTbl[expr];
                // ------------------------------------------------ //
                if (DEBUG) {
                    std::cout << "substituting, and the numbering is: " << num
                              << ", the canon home is: " << canonHome[num][0]
                              << ", the new value is: " << instr->at("dest")
                              << std::endl;
                }
                // ------------------------------------------------ //
                canonHome[num].push_back(instr->at("dest"));
                Var cHome = canonHome[num][0];

                updateExpr(instr, cHome);
            } else {
                num = insertExprNumBinding(expr, instr);
            }
            updateVar2Num(instr->at("dest"), num);
        }
    }

    outfile << brilProg.dump(4);
    outfile.close();

    return EXIT_SUCCESS;
}

Args getArgs(Instr* instr) {
    Args args;
    if (instr->contains("args")) {
        for (const auto& arg : instr->at("args")) {
            args.push_back(arg);
        }
    }
    return args;
}

std::vector<int> findConst(std::map<int, std::string> numConstMap,
                           std::string constStr) {
    std::vector<int> res;
    for (auto& it : numConstMap) {
        if (it.second == constStr) {
            res = {it.first};
            break;
        }
    }
    return res;
}

ExprTuple genExprTuple(Instr* instr) {
    Op op = instr->at("op");
    std::vector<int> nums;
    if (op == "const") {
        if (DEBUG) std::cout << instr->at("value") << std::endl;
        std::string constVal = instr->at("value").dump();
        std::vector<int> constValNums = findConst(num2Const, constVal);
        if (DEBUG) std::cout << "found const" << std::endl;
        if (constValNums.size() == 0) {
            nums = {numbering + 1};
            num2Const.insert({numbering + 1, constVal});
        } else {
            nums = constValNums;
        }
    } else {
        Args args = getArgs(instr);

        for (const auto& arg : args) {
            nums.push_back(
                var2Num[arg]);  // args are guaranteed to exist in var2Num as
                                // long as it's not a const expression, because
                                // otw the program will raise a syntax error
                                // already
        }
    }

    return std::make_tuple(op, nums);
}

int insertExprNumBinding(ExprTuple expr, Instr* instr) {
    numbering += 1;
    exprNumTbl.insert({expr, numbering});
    for (auto& mapIter : canonHome) {
        std::vector<std::string>& canonVars = mapIter.second;
        for (auto vectIter = canonVars.begin(); vectIter != canonVars.end();
             ++vectIter) {
            if (*vectIter == instr->at("dest")) {
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
    std::vector<Var> newCHome = {instr->at("dest")};
    if (DEBUG) {
        // ------------------------------------------------ //
        std::cout << "inserting new variables, and the new numbering is: "
                  << numbering << std::endl;
    }
    // ------------------------------------------------ //
    canonHome.insert({numbering, newCHome});
    return numbering;
}

void updateVar2Num(Var dest, int num) {
    auto iter = var2Num.find(dest);
    if (iter != var2Num.end())
        iter->second = num;
    else {
        var2Num.insert({dest, num});
    }
}

void updateExpr(Instr* instr, Var cHome) {
    json tmpConst = R"( {"op": "const"} )"_json;
    json tmpArg;
    tmpArg["args"] = json::array({cHome});
    instr->update(tmpConst);
    instr->update(tmpArg);
}