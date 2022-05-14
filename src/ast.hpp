#pragma once

#include <string>

#include "ast_equality_comparator.hpp"
#include "codegen.hpp"
#include "types.hpp"

#define EXTRA_METHODS(CLASS)                                                   \
  Code *codegen(CodeGenerator *g) override { return g->generate(this); };      \
  bool isEqual(AstEqualityComparator *c, const AstNode &other)                 \
      const override {                                                         \
    return (typeid(*this) == typeid(other)) &&                                 \
           c->compare(*this, static_cast<const CLASS &>(other));               \
  };

struct AstNode;
using AstNodePtr = std::shared_ptr<AstNode>;

enum BinOpType { ADD, SUB, MUL, DIV };

struct AstNode {
  virtual ~AstNode() = default;
  virtual Code *codegen(CodeGenerator *g) = 0;
  virtual bool isEqual(AstEqualityComparator *c,
                       const AstNode &other) const = 0;
};

struct FloatExprNode : public AstNode {
  FloatExprNode(const std::string &value) : value(value){};
  const std::string value;
  EXTRA_METHODS(FloatExprNode)
};

struct IntegerExprNode : public AstNode {
  IntegerExprNode(const std::string &value) : value(value){};
  const std::string value;
  EXTRA_METHODS(IntegerExprNode)
};

struct BinExprNode : public AstNode {
  BinExprNode(const BinOpType type, const AstNodePtr lhs, const AstNodePtr rhs)
      : type(type), lhs(lhs), rhs(rhs) {}
  BinOpType type;
  AstNodePtr lhs;
  AstNodePtr rhs;
  EXTRA_METHODS(BinExprNode)
};

struct FnProtoNode : public AstNode {
  FnProtoNode(const std::string &name) : name(name){};
  const std::string name;
};

struct FnDefNode : public AstNode {
  FnDefNode(const std::shared_ptr<FnProtoNode> proto, const AstNodePtr body)
      : proto(proto), body(body){};
  std::shared_ptr<FnProtoNode> proto;
  AstNodePtr body;
};

struct VarDeclNode : public AstNode {
  VarDeclNode(const std::string &name, const Type &type, const AstNodePtr expr)
      : name(name), type(type), expr(expr){};
  const std::string name;
  const Type type;
  AstNodePtr expr;
  EXTRA_METHODS(VarDeclNode)
};
