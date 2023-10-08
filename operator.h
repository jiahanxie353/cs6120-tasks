#pragma once

#include "value.h"

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

class BinaryOp : public Operator {
public:
  virtual Value *compute(Value *operand1, Value *operand2) = 0;
};

class UnaryOp : public Operator {
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

class AddOp : public BinaryOp {};

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