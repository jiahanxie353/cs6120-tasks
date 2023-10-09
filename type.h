#pragma once
#include <memory>
#include <string>

class Type {
public:
  static std::shared_ptr<Type> createType(const std::string typeStr);
  virtual ~Type() = default;
  virtual const std::string dump() const = 0;
};

class PrimType : public Type {
public:
  const std::string dump() const override { return "primitive"; }
};

class IntType : public PrimType {
public:
  const std::string dump() const override { return "int"; }
};

class BoolType : public PrimType {
public:
  const std::string dump() const override { return "bool"; }
};

class VoidType : public Type {
public:
  const std::string dump() const override { return "void"; }
};

class LitType : public Type {
public:
  const std::string dump() const override { return "literal"; }
};

class FcnType : public Type {
public:
  const std::string dump() const override { return "function"; }
};

// `isa`, perhaps?
