#pragma once

#include <string>

#include "ast_equality_comparator.hpp"
#include "codegen.hpp"

#define EXTRA_METHODS(CLASS)                                                   \
  Code *codegen(CodeGenerator *g) override { return g->generate(this); };      \
  bool isEqual(AstEqualityComparator *c, const AstNode &other)                 \
      const override {                                                         \
    return (typeid(*this) == typeid(other)) &&                                 \
           c->compare(*this, static_cast<const CLASS &>(other));               \
  };

class AstNode;
using AstNodePtr = std::shared_ptr<AstNode>;

enum BinOpType { ADD, SUB, MUL, DIV };

class AstNode {
public:
  virtual ~AstNode() = default;
  virtual Code *codegen(CodeGenerator *g) = 0;
  virtual bool isEqual(AstEqualityComparator *c,
                       const AstNode &other) const = 0;
};

class FloatExprNode : public AstNode {
public:
  FloatExprNode(const std::string &value) : value(value){};
  const std::string value;

  EXTRA_METHODS(FloatExprNode)
};

class IntegerExprNode : public AstNode {
public:
  IntegerExprNode(const std::string &value) : value(value){};
  const std::string value;

  EXTRA_METHODS(IntegerExprNode)
};

class BinExprNode : public AstNode {
public:
  BinExprNode(const BinOpType type, const AstNodePtr lhs, const AstNodePtr rhs)
      : type(type), lhs(lhs), rhs(rhs) {}
  BinOpType type;
  AstNodePtr lhs;
  AstNodePtr rhs;

  EXTRA_METHODS(BinExprNode)
};

class FnProtoNode : public AstNode {
public:
  FnProtoNode(const std::string &name) : name(name){};
  const std::string name;
};

class FnDefNode : public AstNode {
public:
  FnDefNode(const std::shared_ptr<FnProtoNode> proto, const AstNodePtr body)
      : proto(proto), body(body){};

  std::shared_ptr<FnProtoNode> proto;
  AstNodePtr body;
};
