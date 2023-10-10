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
  Instruction(json *j) : Value(), jsonRepr(j), op(nullptr), value(nullptr) {}
  Instruction(Operator *o, Type *t)
      : Value(t), jsonRepr(nullptr), op(o), value(nullptr) {}
  virtual ~Instruction() override {
    delete op;
    delete value;
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
  // Turns an instruction json to `Instruction`
  void initInstr(json *ij, std::optional<std::vector<ArgValue *>> args) {
    if (ij->contains("type")) {
      std::string typeStr = ij->at("type").get<std::string>();
      type = Type::createType(typeStr);
    } else
      type = std::make_shared<VoidType>();
    customInit(args);
  }
  // Customizes parsing `jsonRepr` to additional details. Derived classes will
  // use their extra attributes as well.
  virtual void customInit(std::optional<std::vector<ArgValue *>>) = 0;
};

// A `ConstInstr` is an instruction that produces a literal value.
// A `ConstInstr`'s `op` field must be the string "const".
class ConstInstr : public Instruction {
public:
  ConstInstr(json *j) : Instruction(j) { initInstr(jsonRepr, std::nullopt); }

  ConstValue *getValue() const override {
    return static_cast<ConstValue *>(value);
  }

  PrimType *getType() const override { return getValue()->getType(); }

protected:
  void customInit(std::optional<std::vector<ArgValue *>>) override;
};

// A `ValueInstr` is an instruction that takes arguments, does some
// computation, and produces a value. It has "dest" and "type" fields
class ValueInstr : public Instruction {
public:
  ValueInstr(json *j, std::optional<std::vector<ArgValue *>> args)
      : Instruction(j) {
    initInstr(jsonRepr, args);
  }
  // Returns the produced value.
  Value *getValue() const override { return value; }
  Value *getDest() const { return dest; }
  Type *getType() const override;

  ~ValueInstr() override { delete dest; }

protected:
  void customInit(std::optional<std::vector<ArgValue *>>) override;

private:
  Value *dest;
  std::optional<std::vector<ArgValue *>> arguments;
};

// An `EffectInstr` is like a `ValueInstr` but it does not produce a value.
class EffectInstr : public Instruction {
public:
  EffectInstr(json *j, std::optional<std::vector<ArgValue *>> args)
      : Instruction(j) {
    initInstr(jsonRepr, args);
  }
  // Returns an empty value.
  EmptyValue *getValue() const override {
    return static_cast<EmptyValue *>(value);
  }

  Type *getType() const override {
    if (jsonRepr->at("op") == "print")
      return dynamic_cast<VoidType *>(type.get());
  }

protected:
  void customInit(std::optional<std::vector<ArgValue *>>) override;

private:
  std::optional<std::vector<ArgValue *>> arguments;
};