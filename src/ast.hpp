#pragma once

#include <memory>
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

struct BlockNode : public AstNode {
  EXTRA_METHODS(BlockNode)
};

struct FnDefNode : public AstNode {
  FnDefNode(const std::string &name, const UzType &returnType,
            const AstNodePtr body)
      : name(name), returnType(returnType), body(body){};
  const std::string name;
  const UzType returnType;
  AstNodePtr body;
  EXTRA_METHODS(FnDefNode)
};

struct VarDeclNode : public AstNode {
  VarDeclNode(const std::string &name, const UzType &type,
              const AstNodePtr initExpr)
      : name(name), type(type), initExpr(initExpr){};
  const std::string name;
  const UzType type;
  AstNodePtr initExpr;
  EXTRA_METHODS(VarDeclNode)
};

struct RootNode : public AstNode {
  std::vector<AstNodePtr> declarations;
  EXTRA_METHODS(RootNode)
};
