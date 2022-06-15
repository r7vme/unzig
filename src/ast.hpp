#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "codegen.hpp"
#include "dotgen.hpp"
#include "scope.hpp"
#include "types.hpp"

#include "ast_node.hpp"

#define AST_NODE_MEMBERS(CLASS)                                                \
  const uint64_t nodeId{curNodeId++};                                          \
  uint64_t getNodeId() const { return nodeId; };                               \
  llvm::Value *codegen(CodeGenerator *g) const { return g->generate(*this); }; \
  void dotgen(DotGenerator *g) const { return g->generate(*this); };           \
  bool isEqual(const CLASS &other) const;                                      \
  bool isEmptyNode() const { return false; };

static uint64_t curNodeId{0};
enum BinOpType { ADD, SUB, MUL, DIV };
using MayBeAstNode = std::optional<AstNode>;

struct EmptyNode {
  const uint64_t nodeId{curNodeId++};
  uint64_t getNodeId() const { return nodeId; };
  llvm::Value *codegen(CodeGenerator *g) const { return g->generate(*this); };
  void dotgen(DotGenerator *g) const { return g->generate(*this); };
  bool isEqual(const EmptyNode &other) const;
  bool isEmptyNode() const { return true; };
};

struct FnCallExprNode {
  const std::string callee;
  FnCallExprNode(const std::string &callee) : callee(callee) {}
  AST_NODE_MEMBERS(FnCallExprNode)
};

struct VarExprNode {
  VarExprNode(const std::string &name) : name(name) {}
  const std::string name;
  AST_NODE_MEMBERS(VarExprNode)
};

struct FloatExprNode {
  FloatExprNode(const std::string &value) : value(value) {}
  const std::string value;
  AST_NODE_MEMBERS(FloatExprNode)
};

struct IntegerExprNode {
  IntegerExprNode(const std::string &value) : value(value) {}
  const std::string value;
  AST_NODE_MEMBERS(IntegerExprNode)
};

struct BinExprNode {
  BinExprNode(const BinOpType type, const AstNode lhs, const AstNode rhs)
      : type(type), lhs(lhs), rhs(rhs) {}
  BinOpType type;
  AstNode lhs, rhs;
  AST_NODE_MEMBERS(BinExprNode)
};

struct AssignStNode {
  AssignStNode(const AstNode lhs, const AstNode rhs) : lhs(lhs), rhs(rhs) {}
  AstNode lhs, rhs;
  AST_NODE_MEMBERS(AssignStNode)
};

struct ReturnStNode {
  ReturnStNode(const AstNode expr) : expr(expr) {}
  AstNode expr;
  AST_NODE_MEMBERS(ReturnStNode)
};

struct BlockNode {
  BlockNode(const std::vector<AstNode> statements) : statements(statements) {}
  std::vector<AstNode> statements;
  AST_NODE_MEMBERS(BlockNode)
};

struct FnDefNode {
  FnDefNode(const std::string &name, const UzType &returnType,
            const AstNode body)
      : name(name), returnType(returnType), body(body) {}
  const std::string name;
  const UzType returnType;
  AstNode body;
  AST_NODE_MEMBERS(FnDefNode)
};

struct VarDeclNode {
  VarDeclNode(const std::string &name, const UzType &type,
              const AstNode initExpr)
      : name(name), type(type), initExpr(initExpr) {}
  const std::string name;
  const UzType type;
  AstNode initExpr;
  AST_NODE_MEMBERS(VarDeclNode)
};

struct RootNode {
  RootNode(const std::vector<AstNode> declarations)
      : declarations(declarations) {}
  std::vector<AstNode> declarations;
  AST_NODE_MEMBERS(RootNode)
};
