#pragma once

#include "codegen.hpp"
#include <string>

#define CODEGEN_METHOD                                                         \
  Code *codegen(CodeGenerator *g) override { return g->generate(this); };

class AstNode;

using AstNodePtr = std::shared_ptr<AstNode>;

enum BinaryOpType { ADD, SUB, MUL, DIV };

class AstNode {
public:
  virtual ~AstNode() = default;
  virtual Code *codegen(CodeGenerator *g) = 0;
};

class IntegerExprNode : public AstNode {
public:
  IntegerExprNode(const std::string &value) : value(value){};
  const std::string value;

  CODEGEN_METHOD
};

class BinaryExprNode : public AstNode {
public:
  BinaryExprNode(const BinaryOpType type, const AstNodePtr lhs,
                 const AstNodePtr rhs)
      : type(type), lhs(lhs), rhs(rhs) {}
  BinaryOpType type;
  AstNodePtr lhs;
  AstNodePtr rhs;

  CODEGEN_METHOD
};
