#pragma once

#include "type.h"
#include <memory>
#include <string>

// An object is a bril `Value` iff it can be used as an operand
class Value {
public:
  Value() : type(nullptr) {}
  Value(Type *t) : type(t) {}
  virtual ~Value() = default;

  virtual Type *getType() const { return type.get(); }
  std::shared_ptr<Type> getTypeSharedPtr() const { return type; }
  virtual const std::string dump() const = 0;

protected:
  std::shared_ptr<Type> type;
};

// A wrapper around empty values. An `EmptyValue` has type `VoidType`.
class EmptyValue : public Value {
  EmptyValue() : Value(new VoidType()) {}

  VoidType *getType() const override {
    return static_cast<VoidType *>(type.get());
  }

  const std::string dump() const override { return std::string(); }
};

// A `Constant` value is either an `int` or a `bool`
class ConstValue : public Value {
public:
  ConstValue(Type *t) : Value(t) {}
  PrimType *getType() const override {
    return static_cast<PrimType *>(type.get());
  }
};

class LitValue : public Value {
public:
  LitValue(Type *lt, std::string d)
      : Value(static_cast<LitType *>(lt)), data(d) {}

  LitType *getType() const override {
    return static_cast<LitType *>(type.get());
  }

  std::string getValue() const { return data; }

  const std::string dump() const override { return data; }

private:
  std::string data;
};

class IntValue : public ConstValue {
public:
  IntValue(int d) : ConstValue(new IntType()) { data = d; }
  IntValue(std::string sd) : ConstValue(new IntType()) { data = std::stoi(sd); }

  IntType *getType() const override {
    return static_cast<IntType *>(type.get());
  }

  const std::string dump() const override { return std::to_string(data); }

  int getData() const { return data; }

private:
  int data;
};

class BoolValue : public ConstValue {
public:
  BoolValue(bool d) : ConstValue(new BoolType()) { data = d; }
  BoolValue(std::string sd);

  BoolType *getType() const override {
    return static_cast<BoolType *>(type.get());
  }

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