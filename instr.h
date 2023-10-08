#pragma once

#include <nlohmann/json.hpp>
#include <optional>

#include "operator.h"
#include "type.h"
#include "value.h"

// Not a good practice, but let's use it
using json = nlohmann::json;

// An `Instruction` represents a unit of computational work. Every instruction
// must have `op` field. I'm also forcing it to have `type`, if the json
// representation doesn't have `type` field, we'll just cast it to `VoidType`.
// There are three kinds of instructions: constants, value operations, and
// effect operations.
// Note: An `Instruction` is a `Value` because it can be used.
class Instruction : public Value {
public:
  Instruction(json *j) { jsonRepr = j; }
  Instruction(Operator *o, Type *t) : Value(t), op(o) {}
  virtual ~Instruction() {
    delete jsonRepr;
    delete op;
  }

  Operator *getOperator() const { return op; }
  // Returns the value of the instruction that can be used by others.
  virtual Value *getValue() const = 0;
  const std::string dump() const override { return getValue()->dump(); }

  const json *getJson() const { return jsonRepr; }

protected:
  json *jsonRepr;
  Operator *op;
  Value *value;
  virtual void jsonToInstr(json *) = 0;
};

// A `ConstInstr` is an instruction that produces a literal value.
// A `ConstInstr`'s `op` field must be the string "const".
class ConstInstr : public Instruction {
public:
  ConstInstr(json *j) : Instruction(j) { jsonToInstr(std::move(jsonRepr)); }

  ConstValue *getValue() const override {
    return static_cast<ConstValue *>(value);
  }

  PrimType *getType() const override { return getValue()->getType(); }

protected:
  void jsonToInstr(json *j) override {
    std::string typeStr = j->at("type").get<std::string>();
    std::string opStr = j->at("op").get<std::string>();

    if (typeStr == "int") {
      type = new IntType();
      int valueInt = j->at("value").get<int>();
      value = new IntValue(type, valueInt);
      op = new ConstOp();
    } else if (typeStr == "bool") {
      type = new BoolType();
      bool valueBool = j->at("value").get<bool>();
      value = new BoolValue(type, valueBool);
      op = new ConstOp();
    } else {
      throw std::runtime_error(
          "Constant instructions have to be either int or boolean.\n");
    }
  }
};

// A `ValueInstr` is an instruction that takes arguments, does some
// computation, and produces a value. It has "dest" and "type" fields
class ValueInstr : public Instruction {
public:
  // Returns the produced value.
  Value *getValue() const override { return value; }
  Value *getDest() const { return dest; }
  Type *getType() const override{};

protected:
  void jsonToInstr(json *j) override {}

private:
  Value *dest;
  Type *type;
  std::optional<std::vector<Value *>> arguments;
};

// An `EffectInstr` is like a `ValueInstr` but it does not produce a value.
class EffectInstr : public Instruction {
public:
  // Returns an empty value.
  EmptyValue *getValue() const override {
    return static_cast<EmptyValue *>(value);
  }
  Value *getDest() const { return dest; }
  Type *getType() const override{};

protected:
  void jsonToInstr(json *j) override {}

private:
  Value *dest;
  Type *type;
  std::optional<std::vector<Value *>> arguments;
};