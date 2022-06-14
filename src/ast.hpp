#pragma once

#include <memory>
#include <string>

#include "codegen.hpp"
#include "dotgen.hpp"
#include "scope.hpp"
#include "types.hpp"

#include "ast_node.hpp"

#define COMMON_MEMBERS(CLASS)                                                  \
  llvm::Value *codegen(CodeGenerator *g) const { return g->generate(*this); }; \
  void dotgen(DotGenerator *g) const { return g->generate(*this); }; \
  bool isEqual(const CLASS& other) const;

enum BinOpType { ADD, SUB, MUL, DIV };

using MayBeAstNode = std::optional<AstNode>;

struct EmptyNode {
  COMMON_MEMBERS(EmptyNode)
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
  AssignStNode(const AstNode lhs, const AstNode rhs) : lhs(lhs), rhs(rhs) {}
  AstNode lhs, rhs;
  COMMON_MEMBERS(AssignStNode)
};

struct ReturnStNode {
  ReturnStNode(const AstNode expr) : expr(expr) {}
  AstNode expr;
  COMMON_MEMBERS(ReturnStNode)
};

struct BlockNode {
  BlockNode(const std::vector<AstNode> statements) : statements(statements) {}
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

//bool operator==(const FloatExprNode &lhs, const FloatExprNode &rhs);
//bool operator==(const IntegerExprNode &lhs, const IntegerExprNode &rhs);
//bool operator==(const BinExprNode &lhs, const BinExprNode &rhs);
//bool operator==(const VarDeclNode &lhs, const VarDeclNode &rhs);
//bool operator==(const FnDefNode &lhs, const FnDefNode &rhs);
//bool operator==(const BlockNode &lhs, const BlockNode &rhs);
//bool operator==(const RootNode &lhs, const RootNode &rhs);
//bool operator==(const AssignStNode &lhs, const AssignStNode &rhs);
//bool operator==(const ReturnStNode &lhs, const ReturnStNode &rhs);
//bool operator==(const VarExprNode &lhs, const VarExprNode &rhs);
//bool operator==(const FnCallExprNode &lhs, const FnCallExprNode &rhs);
//bool operator==(const EmptyNode &lhs, const EmptyNode &rhs);
