#pragma once

#include <memory>
#include <string>

#include "ast_equality_comparator.hpp"
#include "codegen.hpp"
#include "dotgen.hpp"
#include "types.hpp"

#define EXTRA_METHODS(CLASS)                                                   \
  llvm::Value *codegen(CodeGenerator *g) override {                            \
    return g->generate(this);                                                  \
  };                                                                           \
  void dotgen(DotGenerator *g, std::string &output) override {                 \
    return g->generate(this, output);                                          \
  };                                                                           \
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
  virtual llvm::Value *codegen(CodeGenerator *g) = 0;
  virtual void dotgen(DotGenerator *g, std::string &output) = 0;
  virtual bool isEqual(AstEqualityComparator *c,
                       const AstNode &other) const = 0;
};

struct FnCallExprNode : public AstNode {
  FnCallExprNode(const std::string &callee) : callee(callee){};
  const std::string callee;
  EXTRA_METHODS(FnCallExprNode)
};

struct VarExprNode : public AstNode {
  VarExprNode(const std::string &name) : name(name){};
  const std::string name;
  EXTRA_METHODS(VarExprNode)
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
  AstNodePtr lhs, rhs;
  EXTRA_METHODS(BinExprNode)
};

struct AssignStNode : public AstNode {
  AssignStNode(const AstNodePtr lhs, const AstNodePtr rhs)
      : lhs(lhs), rhs(rhs) {}
  AstNodePtr lhs, rhs;
  EXTRA_METHODS(AssignStNode)
};

struct ReturnStNode : public AstNode {
  ReturnStNode(const AstNodePtr expr) : expr(expr) {}
  AstNodePtr expr;
  EXTRA_METHODS(ReturnStNode)
};

struct BlockNode : public AstNode {
  BlockNode(const std::vector<AstNodePtr> statements)
      : statements(statements) {}
  std::vector<AstNodePtr> statements;
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
  RootNode(const std::vector<AstNodePtr> declarations)
      : declarations(declarations) {}
  std::vector<AstNodePtr> declarations;
  EXTRA_METHODS(RootNode)
};
