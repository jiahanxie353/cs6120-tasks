#pragma once

class Type {};

class PrimType : public Type {};

class IntType : public PrimType {};

class BoolType : public PrimType {};

class VoidType : public Type {};

class FcnType : public Type {};

// `isa`, perhaps?
