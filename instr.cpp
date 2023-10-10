#include "instr.h"
#include <iostream>
#include <set>

using std::string;
using std::vector;

std::set<string> ArithOpStr = {"add", "mul", "sub", "div"};

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

    arguments = args;

    dest = new LitValue(new LitType(), jsonRepr->at("dest").get<string>());
  } else if (ArithOpStr.find(opStr) != ArithOpStr.end()) {
    dest = new LitValue(new LitType(), jsonRepr->at("dest").get<string>());

    auto jsonArgs = jsonRepr->at("args").get<vector<string>>();
    assert(jsonArgs.size() == 2 && args.value().size() == 2 &&
           "Arithmetic operation instructions have two arguments.\n");

    arguments = args;
    ArgValue *operand1 = args.value()[0];
    ArgValue *operand2 = args.value()[1];

    string typeStr = jsonRepr->at("type").get<string>();
    assert(typeStr == "int" &&
           "Arithmetic instructions have to assign int values.\n");

    assert(operand1->getType()->dump() == "int" &&
           operand2->getType()->dump() == "int" &&
           "Both operands types have to be int.\n");
    type = operand1->getTypeSharedPtr();
    IntValue *intValue1 = dynamic_cast<IntValue *>(operand1->getValue());
    IntValue *intValue2 = dynamic_cast<IntValue *>(operand2->getValue());

    if (opStr == "add") {
      op = new AddOp();
      value = dynamic_cast<AddOp *>(op)->compute(intValue1, intValue2);
    } else if (opStr == "mul") {
      op = new MulOp();
      value = dynamic_cast<MulOp *>(op)->compute(intValue1, intValue2);
    } else if (opStr == "sub") {
      op = new SubOp();
      value = dynamic_cast<SubOp *>(op)->compute(intValue1, intValue2);
    } else if (opStr == "div") {
      op = new DivOp();
      value = dynamic_cast<DivOp *>(op)->compute(intValue1, intValue2);
    } else {
      throw std::runtime_error("Invalid arith operation: " + opStr + "!\n");
    }
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