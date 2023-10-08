#pragma once

#include "type.h"
#include <string>

// An object is a bril `Value` iff it can be used as an operand
class Value {
public:
  Value() : type(nullptr) {}
  Value(Type *t) : type(t) {}
  virtual ~Value() { delete type; }

  virtual Type *getType() const = 0;
  virtual const std::string dump() const = 0;

protected:
  Type *type;
};

// A wrapper around empty values. An `EmptyValue` has type `VoidType`.
class EmptyValue : public Value {
  EmptyValue() : Value(new VoidType()) {}

  VoidType *getType() const override { return static_cast<VoidType *>(type); }

  const std::string dump() const override { return std::string(); }
};

// A `Constant` value is either an `int` or a `bool`
class ConstValue : public Value {
public:
  ConstValue(Type *pt) : Value(static_cast<PrimType *>(pt)) {}

  PrimType *getType() const override { return static_cast<PrimType *>(type); }
};

class FcnArg : public Value {};

class IntValue : public ConstValue {
public:
  IntValue(Type *it, int d) : ConstValue(static_cast<IntType *>(it)), data(d) {}
  IntValue(Type *it, std::string sd)
      : ConstValue(static_cast<IntType *>(it)), data(std::stoi(sd)) {}

  IntType *getType() const override { return static_cast<IntType *>(type); }

  const std::string dump() const override { return std::to_string(data); }

  int getData() const { return data; }

private:
  int data;
};

class BoolValue : public ConstValue {
public:
  BoolValue(Type *bt, bool d);
  BoolValue(Type *bt, std::string sd);

  BoolType *getType() const override { return static_cast<BoolType *>(type); }

  const std::string dump() const override {
    if (data)
      return "true";
    else
      return "false";
  }

  bool getData() const { return data; }

private:
  bool data;
};