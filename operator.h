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
  virtual Value *compute(IntValue *operand1, IntValue *operand2) = 0;
  virtual Value *compute(BoolValue *operand1, BoolValue *operand2) = 0;
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

class PhiOp : public Operator {};

class MemOp : public Operator {};

// ===-----------------------------===//
// Binary Operators
// ===-----------------------------===//

class AddOp : public BinaryOp {
public:
  AddOp() : Operator(Add) {}

  IntValue *compute(IntValue *operand1, IntValue *operand2) override {
    int result = operand1->getData() + operand2->getData();

    return new IntValue(result);
  }
  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }
  const std::string dump() const override { return "add"; }
};

class MulOp : public BinaryOp {
public:
  MulOp() : Operator(Mul) {}
  IntValue *compute(IntValue *operand1, IntValue *operand2) override {
    int result = operand1->getData() * operand2->getData();

    return new IntValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "multiply"; }
};

class SubOp : public BinaryOp {
public:
  SubOp() : Operator(Sub) {}

  IntValue *compute(IntValue *operand1, IntValue *operand2) override {
    int result = operand1->getData() - operand2->getData();

    return new IntValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "subtract"; }
};

class DivOp : public BinaryOp {
public:
  DivOp() : Operator(Div) {}

  IntValue *compute(IntValue *operand1, IntValue *operand2) override {
    int result = operand1->getData() / operand2->getData();

    return new IntValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "divide"; }
};

class EqOp : public BinaryOp {
public:
  EqOp() : Operator(Eq) {}

  BoolValue *compute(IntValue *operand1, IntValue *operand2) override {
    bool result = operand1->getData() == operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "equal"; }
};

class LtOp : public BinaryOp {
public:
  LtOp() : Operator(Lt) {}

  BoolValue *compute(IntValue *operand1, IntValue *operand2) override {
    bool result = operand1->getData() < operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "less than"; }
};

class GtOp : public BinaryOp {
public:
  GtOp() : Operator(Gt) {}

  BoolValue *compute(IntValue *operand1, IntValue *operand2) override {
    bool result = operand1->getData() > operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "greater than"; }
};

class LeOp : public BinaryOp {
public:
  LeOp() : Operator(Le) {}

  BoolValue *compute(IntValue *operand1, IntValue *operand2) override {
    bool result = operand1->getData() <= operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "less than or equal"; }
};

class GeOp : public BinaryOp {
public:
  GeOp() : Operator(Ge) {}

  BoolValue *compute(IntValue *operand1, IntValue *operand2) override {
    bool result = operand1->getData() >= operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(BoolValue *operand1, BoolValue *operand2) override {
    throw std::runtime_error("Operands should be IntType\n");
  }

  const std::string dump() const override { return "greater than or equal"; }
};

class AndOp : public BinaryOp {
public:
  AndOp() : Operator(And) {}

  BoolValue *compute(BoolValue *operand1, BoolValue *operand2) override {
    bool result = operand1->getData() && operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(IntValue *operand1, IntValue *operand2) override {
    throw std::runtime_error("Operands should be BoolType\n");
  }

  const std::string dump() const override { return "and"; }
};

class OrOp : public BinaryOp {
public:
  OrOp() : Operator(Or) {}

  BoolValue *compute(BoolValue *operand1, BoolValue *operand2) override {
    bool result = operand1->getData() || operand2->getData();

    return new BoolValue(result);
  }

  Value *compute(IntValue *operand1, IntValue *operand2) override {
    throw std::runtime_error("Operands should be BoolType\n");
  }

  const std::string dump() const override { return "or"; }
};

// ===-----------------------------===//
// Unary Operators
// ===-----------------------------===//

class IdOp : public UnaryOp {
public:
  IdOp() : Operator(Id) {}
  const std::string dump() const override { return "id"; }

  Value *compute(Value *operand) override { return operand; }
};

class NotOp : public UnaryOp {
public:
  NotOp() : Operator(Not) {}
  const std::string dump() const override { return "not"; }

  BoolValue *compute(Value *operand) override {
    bool invertData = !dynamic_cast<BoolValue *>(operand)->getData();
    return new BoolValue(invertData);
  }
};

// ===-----------------------------===//
// Control Operators
// ===-----------------------------===//

class JmpOp : CtrlOp {};

class BrOp : CtrlOp {};

// ===-----------------------------===//
// Misc Operators
// ===-----------------------------===//
class PrintOp : public Operator {
public:
  PrintOp() : Operator(Print) {}

  const std::string dump() const override { return "print"; }

  void execute(Value *v) const { std::cout << v->dump() << std::endl; }
};

class Nop : public Operator {};

// ===-----------------------------===//
// Function Operators
// ===-----------------------------===//

class CallOp : FcnOp {};

class RetOp : FcnOp {};