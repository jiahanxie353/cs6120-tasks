#include "instr.h"

using std::string;
using std::vector;

void ConstInstr::customInit(std::optional<vector<ArgValue *>> args) {
  string typeStr = jsonRepr->at("type").get<string>();
  assert(typeStr == "int" ||
         typeStr == "bool" &&
             "Constant instructions have to be either int or boolean.\n");

  op = new ConstOp();
  if (typeStr == "int")
    value = new IntValue(jsonRepr->at("value").get<int>());
  else
    value = new BoolValue(jsonRepr->at("value").get<bool>());
}

void ValueInstr::customInit(std::optional<vector<ArgValue *>> args) {
  string opStr = jsonRepr->at("op").get<string>();
  if (opStr == "id") {
    op = new IdOp();

    string typeStr = jsonRepr->at("type").get<string>();
    assert(typeStr == "int" ||
           typeStr == "bool" &&
               "Copy instructions have to be either int or boolean.\n");

    auto jsonArgs = jsonRepr->at("args").get<vector<string>>();
    assert(jsonArgs.size() == 1 && args.value().size() == 1 &&
           "copy instruction only has one argument.\n");

    ArgValue *copied = args.value()[0];
    if (typeStr == "int") {
      type = copied->getTypeSharedPtr();
      value = new IntValue(copied->getValue()->dump());
    } else {
      type = copied->getTypeSharedPtr();
      value = new BoolValue(copied->getValue()->dump());
    }

    arguments = std::make_optional<vector<ArgValue *>>({copied});

    LitType *lit = new LitType();
    dest = new LitValue(lit, jsonRepr->at("dest").get<string>());
  } else
    throw std::runtime_error("Other value instructions not implemented yet!\n");
}

Type *ValueInstr::getType() const {
  switch (op->getOpcode()) {
  case Id:
    return getValue()->getType();
    break;

  default:
    break;
  }
}