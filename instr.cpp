#include "instr.h"
#include <iostream>
#include <set>

using std::string;
using std::vector;

std::set<string> ArithOpStr = {"add", "mul", "sub", "div"};
std::set<string> CmpOpStr = {"eq", "lt", "gt", "le", "ge"};
std::set<string> LogicOpStr = {"not", "and", "or"};

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
  } else if (ArithOpStr.find(opStr) != ArithOpStr.end() ||
             CmpOpStr.find(opStr) != CmpOpStr.end()) {
    dest = new LitValue(new LitType(), jsonRepr->at("dest").get<string>());

    auto jsonArgs = jsonRepr->at("args").get<vector<string>>();
    assert(
        jsonArgs.size() == 2 && args.value().size() == 2 &&
        "Arithmetic/Comparison operation instructions have two arguments.\n");

    arguments = args;
    ArgValue *operand1 = args.value()[0];
    ArgValue *operand2 = args.value()[1];

    assert(operand1->getType()->dump() == "int" &&
           operand2->getType()->dump() == "int" &&
           "Both operands types have to be int.\n");
    type = operand1->getTypeSharedPtr();
    IntValue *intValue1 = dynamic_cast<IntValue *>(operand1->getValue());
    IntValue *intValue2 = dynamic_cast<IntValue *>(operand2->getValue());

    string typeStr = jsonRepr->at("type").get<string>();
    if (ArithOpStr.find(opStr) != ArithOpStr.end()) {
      assert(typeStr == "int" &&
             "Arithmetic instructions have to assign int values.\n");
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
    } else {
      assert(typeStr == "bool" &&
             "Comparison instructions have to assign bool values.\n");
      if (opStr == "eq") {
        op = new EqOp();
        value = dynamic_cast<EqOp *>(op)->compute(intValue1, intValue2);
      } else if (opStr == "lt") {
        op = new LtOp();
        value = dynamic_cast<LtOp *>(op)->compute(intValue1, intValue2);
      } else if (opStr == "gt") {
        op = new GtOp();
        value = dynamic_cast<GtOp *>(op)->compute(intValue1, intValue2);
      } else if (opStr == "le") {
        op = new LeOp();
        value = dynamic_cast<LeOp *>(op)->compute(intValue1, intValue2);
      } else if (opStr == "ge") {
        op = new GeOp();
        value = dynamic_cast<GeOp *>(op)->compute(intValue1, intValue2);
      } else {
        throw std::runtime_error("Invalid compare operation: " + opStr + "!\n");
      }
    }
  } else if (LogicOpStr.find(opStr) != LogicOpStr.end()) {
    dest = new LitValue(new LitType(), jsonRepr->at("dest").get<string>());

    string typeStr = jsonRepr->at("type").get<string>();
    assert(typeStr == "bool" &&
           "Logical instructions have to assign bool values.\n");

    arguments = args;
    auto jsonArgs = jsonRepr->at("args").get<vector<string>>();

    if (opStr == "not") {
      assert(jsonArgs.size() == 1 && args.value().size() == 1 &&
             "Not operation instructions have one argument.\n");

      ArgValue *operand = args.value()[0];

      assert(operand->getType()->dump() == "bool" &&
             "Operand type has to be bool.\n");

      type = operand->getTypeSharedPtr();

      BoolValue *boolValue = dynamic_cast<BoolValue *>(operand->getValue());

      op = new NotOp();
      value = dynamic_cast<NotOp *>(op)->compute(boolValue);
    } else if (opStr == "and" || opStr == "or") {
      assert(jsonArgs.size() == 2 && args.value().size() == 2 &&
             "And/Or operation instructions have two arguments.\n");

      ArgValue *operand1 = args.value()[0];
      ArgValue *operand2 = args.value()[1];

      assert(operand1->getType()->dump() == "bool" &&
             operand2->getType()->dump() == "bool" &&
             "Both operands types have to be bool.\n");

      type = operand1->getTypeSharedPtr();

      BoolValue *boolValue1 = dynamic_cast<BoolValue *>(operand1->getValue());
      BoolValue *boolValue2 = dynamic_cast<BoolValue *>(operand2->getValue());

      if (opStr == "and") {
        op = new AndOp();
        value = dynamic_cast<AndOp *>(op)->compute(boolValue1, boolValue2);
      } else {
        op = new OrOp();
        value = dynamic_cast<OrOp *>(op)->compute(boolValue1, boolValue2);
      }
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