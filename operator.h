#pragma once

#include "value.h"
#include <cassert>
#include <iostream>

enum OpCode {
  Const,
  Id,
  Add,
  Mul,
  Sub,
  Div,
  Eq,
  Lt,
  Gt,
  Le,
  Ge,
  And,
  Or,
  Not,
  Jmp,
  Br,
  Call,
  Ret,
  Print,
  Nop,
  Phi
};

class Operator {
public:
  Operator(OpCode op) : oc(op) {}
  virtual ~Operator() = default;
  virtual const std::string dump() const = 0;
  OpCode getOpcode() const { return oc; }

protected:
  OpCode oc;
};

class ConstOp : public Operator {
public:
  ConstOp() : Operator(Const) {}
  // Constant instruction's `op` field must be string "const"
  const std::string dump() const override { return "const"; }
};

class BinaryOp : virtual public Operator {
public:
  virtual IntValue *compute(IntValue *operand1, IntValue *operand2) = 0;
};

class UnaryOp : virtual public Operator {
public:
  virtual Value *compute(Value *operand) = 0;
};

// `CtrlOp` includes operatotrs that involve the control flow, such as "jmp",
// "br"
class CtrlOp : public Operator {};

// `FcnOp` includes operators that involve function calls, such as "call", "ret"
class FcnOp : public Operator {};

class Nop : public Operator {};

class PhiOp : public Operator {};

class MemOp : public Operator {};

// ===-----------------------------===//
// Binary Operators
// ===-----------------------------===//

class AddOp : public BinaryOp {
public:
  AddOp() : Operator(Add) {}
  IntValue *compute(IntValue *operand1, IntValue *operand2) override {
    int addResult = operand1->getData() + operand2->getData();

    return new IntValue(addResult);
  }

  const std::string dump() const override { return "add"; }
};

class MulOp : public BinaryOp {};

class SubOp : public BinaryOp {};

class DivOp : public BinaryOp {};

class EqOp : public BinaryOp {};

class LtOp : public BinaryOp {};

class LeOp : public BinaryOp {};

class GeOp : public BinaryOp {};

class AndOp : public BinaryOp {};

class OrOp : public BinaryOp {};

// ===-----------------------------===//
// Unary Operators
// ===-----------------------------===//

class IdOp : public UnaryOp {
public:
  IdOp() : Operator(Id) {}
  const std::string dump() const override { return "id"; }

  Value *compute(Value *operand) override { return operand; }
};

class NotOp : public UnaryOp {};

// ===-----------------------------===//
// Control Operators
// ===-----------------------------===//

class JmpOp : CtrlOp {};

class BrOp : CtrlOp {};

// ===-----------------------------===//
// Function Operators
// ===-----------------------------===//

class CallOp : FcnOp {};

class RetOp : FcnOp {};