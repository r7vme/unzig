#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "ast_node.hpp"
#include "codegen.hpp"
#include "dotgen.hpp"
#include "scope.hpp"
#include "sema.hpp"
#include "types.hpp"

#define COMMON_MEMBERS(CLASS)                                                                      \
  size_t sourcePos{0};                                                                             \
  Scope scope{nullptr};                                                                            \
  void setScope(Scope newScope) { scope = newScope; };                                             \
  Scope &getScope() { return scope; };                                                             \
  const uint64_t nodeId{curNodeId++};                                                              \
  uint64_t getNodeId() const { return nodeId; };                                                   \
  llvm::Value *codegen(Codegen *g) const { return g->generate(*this); };                           \
  void dotgen(DotGenerator *g) const { return g->generate(*this); };                               \
  void sema(SemanticAnalyzer *g) { return g->analyze(*this); };                                    \
  bool isEqual(const CLASS &other) const;

#define IS_EMPTY_NODE_FALSE                                                                        \
  bool isEmptyNode() const { return false; };

#define AST_NODE_MEMBERS(CLASS)                                                                    \
  COMMON_MEMBERS(CLASS)                                                                            \
  IS_EMPTY_NODE_FALSE

static uint64_t curNodeId{0};
enum BinOpType { ADD, SUB, MUL, DIV };
enum PrefixOpType { NOT };
using MayBeAstNode = std::optional<AstNode>;

struct EmptyNode {
  bool isEmptyNode() const { return true; };
  COMMON_MEMBERS(EmptyNode);
};

struct VarExprNode {
  const std::string name;

  Symbol varSymbol{nullptr};

  VarExprNode(const std::string &name, const size_t sourcePos) : name(name), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(VarExprNode)
};

struct FloatExprNode {
  const std::string value;

  FloatExprNode(const std::string &value, const size_t sourcePos)
      : value(value), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(FloatExprNode)
};

struct IntegerExprNode {
  const std::string value;

  IntegerExprNode(const std::string &value, const size_t sourcePos)
      : value(value), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(IntegerExprNode)
};

struct BoolExprNode {
  const bool value;

  BoolExprNode(const bool value, const size_t sourcePos) : value(value), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(BoolExprNode)
};

struct BinExprNode {
  BinOpType type;
  AstNode lhs, rhs;

  BinExprNode(const BinOpType type, const AstNode lhs, const AstNode rhs, const size_t sourcePos)
      : type(type), lhs(lhs), rhs(rhs), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(BinExprNode)
};

struct OrExprNode {
  std::vector<AstNode> expressions;

  OrExprNode(const std::vector<AstNode> expressions, const size_t sourcePos)
      : expressions(expressions), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(OrExprNode)
};

struct AndExprNode {
  std::vector<AstNode> expressions;

  AndExprNode(const std::vector<AstNode> expressions, const size_t sourcePos)
      : expressions(expressions), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(AndExprNode)
};

struct PrefixExprNode {
  std::vector<PrefixOpType> operators;
  AstNode expr;

  PrefixExprNode(const std::vector<PrefixOpType> operators, const AstNode expr,
                 const size_t sourcePos)
      : operators(operators), expr(expr), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(PrefixExprNode)
};

struct AssignStNode {
  const std::string varName;
  AstNode expr;

  Symbol varSymbol{nullptr};

  AssignStNode(const std::string &varName, const AstNode expr, const size_t sourcePos)
      : varName(varName), expr(expr), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(AssignStNode)
};

struct ReturnStNode {
  AstNode expr;

  ReturnStNode(const AstNode expr, const size_t sourcePos) : expr(expr), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(ReturnStNode)
};

struct IfStNode {
  AstNode ifCondition;
  AstNode thenBlock;
  AstNode elseBlock;

  IfStNode(const AstNode condition, const AstNode block, const AstNode elseStatement,
           const size_t sourcePos)
      : ifCondition(condition), thenBlock(block), elseBlock(elseStatement), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(IfStNode)
};

struct BlockNode {
  std::vector<AstNode> statements;

  BlockNode(const std::vector<AstNode> statements, const size_t sourcePos)
      : statements(statements), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(BlockNode)
};

struct FnCallExprNode {
  const std::string callee;
  std::vector<AstNode> arguments;

  FnCallExprNode(const std::string &callee, const std::vector<AstNode> arguments,
                 const size_t sourcePos)
      : callee(callee), arguments(arguments), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(FnCallExprNode);
};

struct FnDefNode {
  const std::string name;
  const std::string returnTypeName;
  std::vector<AstNode> parameters;
  AstNode body;

  UzType returnType;

  FnDefNode(const std::string &name, const std::string &returnTypeName,
            const std::vector<AstNode> parameters, const AstNode body, const size_t sourcePos)
      : name(name), returnTypeName(returnTypeName), parameters(parameters), body(body),
        sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(FnDefNode)
};

struct FnParamNode {
  const std::string name;
  const std::string typeName;

  UzType type;
  Symbol symbol{nullptr};

  FnParamNode(const std::string &name, const std::string &typeName, const size_t sourcePos)
      : name(name), typeName(typeName), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(FnParamNode);
};

struct VarDeclNode {
  const std::string name;
  const std::string typeName;
  AstNode initExpr;

  UzType type;
  Symbol symbol{nullptr};

  VarDeclNode(const std::string &name, const std::string &typeName, const AstNode initExpr,
              const size_t sourcePos)
      : name(name), typeName(typeName), initExpr(initExpr), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(VarDeclNode)
};

struct RootNode {
  std::vector<AstNode> declarations;

  RootNode(const std::vector<AstNode> declarations, const size_t sourcePos)
      : declarations(declarations), sourcePos(sourcePos) {}
  AST_NODE_MEMBERS(RootNode)
};
