#pragma once

#include <memory>
#include <string>

#include "codegen.hpp"
#include "dotgen.hpp"
#include "scope.hpp"
#include "types.hpp"

#include "ast_node.hpp"

#define COMMON_MEMBERS(CLASS)                                                  \
  llvm::Value *codegen(CodeGenerator *g) { return g->generate(*this); };        \
  void dotgen(DotGenerator *g) { return g->generate(*this); };                  \

enum BinOpType { ADD, SUB, MUL, DIV };

using MayBeAstNode = std::optional<AstNode>;

struct EmptyNode {
  COMMON_MEMBERS(EmptyExprNode)
};

struct FnCallExprNode {
  FnCallExprNode(const std::string &callee) : callee(callee){};
  const std::string callee;
  COMMON_MEMBERS(FnCallExprNode)
};

struct VarExprNode {
  VarExprNode(const std::string &name) : name(name){};
  const std::string name;
  COMMON_MEMBERS(VarExprNode)
};

struct FloatExprNode {
  FloatExprNode(const std::string &value) : value(value){};
  const std::string value;
  COMMON_MEMBERS(FloatExprNode)
};

struct IntegerExprNode {
  IntegerExprNode(const std::string &value) : value(value){};
  const std::string value;
  COMMON_MEMBERS(IntegerExprNode)
};

struct BinExprNode {
  BinExprNode(const BinOpType type, const AstNode lhs, const AstNode rhs)
      : type(type), lhs(lhs), rhs(rhs) {}
  BinOpType type;
  AstNode lhs, rhs;
  COMMON_MEMBERS(BinExprNode)
};

struct AssignStNode {
  AssignStNode(const AstNode lhs, const AstNode rhs)
      : lhs(lhs), rhs(rhs) {}
  AstNode lhs, rhs;
  COMMON_MEMBERS(AssignStNode)
};

struct ReturnStNode {
  ReturnStNode(const AstNode expr) : expr(expr) {}
  AstNode expr;
  COMMON_MEMBERS(ReturnStNode)
};

struct BlockNode {
  BlockNode(const std::vector<AstNode> statements)
      : statements(statements) {}
  std::vector<AstNode> statements;
  COMMON_MEMBERS(BlockNode)
};

struct FnDefNode {
  FnDefNode(const std::string &name, const UzType &returnType,
            const AstNode body)
      : name(name), returnType(returnType), body(body){};
  const std::string name;
  const UzType returnType;
  AstNode body;
  COMMON_MEMBERS(FnDefNode)
};

struct VarDeclNode {
  VarDeclNode(const std::string &name, const UzType &type,
              const AstNode initExpr)
      : name(name), type(type), initExpr(initExpr){};
  const std::string name;
  const UzType type;
  AstNode initExpr;
  COMMON_MEMBERS(VarDeclNode)
};

struct RootNode {
  RootNode(const std::vector<AstNode> declarations)
      : declarations(declarations) {}
  std::vector<AstNode> declarations;
  COMMON_MEMBERS(RootNode)
};
