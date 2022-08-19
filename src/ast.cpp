#include "ast.hpp"

bool PrefixExprNode::isEqual(const PrefixExprNode &other) const {
  return operators == other.operators && expr == other.expr;
}

bool BoolExprNode::isEqual(const BoolExprNode &other) const { return value == other.value; }

bool IntegerExprNode::isEqual(const IntegerExprNode &other) const { return value == other.value; }

bool FloatExprNode::isEqual(const FloatExprNode &other) const { return value == other.value; }

bool BinExprNode::isEqual(const BinExprNode &other) const {
  return type == other.type && lhs == other.lhs && rhs == other.rhs;
}

bool VarDeclNode::isEqual(const VarDeclNode &other) const {
  return name == other.name && typeName == other.typeName && initExpr == other.initExpr;
}

bool FnDefNode::isEqual(const FnDefNode &other) const {
  return name == other.name && returnTypeName == other.returnTypeName && body == other.body;
}

bool BlockNode::isEqual(const BlockNode &other) const { return statements == other.statements; }

bool RootNode::isEqual(const RootNode &other) const { return declarations == other.declarations; }

bool AssignStNode::isEqual(const AssignStNode &other) const {
  return varName == other.varName && expr == other.expr;
}

bool ReturnStNode::isEqual(const ReturnStNode &other) const { return expr == other.expr; }

bool IfStNode::isEqual(const IfStNode &other) const {
  return ifCondition == other.ifCondition && thenBlock == other.thenBlock &&
         elseBlock == other.elseBlock;
}

bool VarExprNode::isEqual(const VarExprNode &other) const { return name == other.name; }

bool FnCallExprNode::isEqual(const FnCallExprNode &other) const { return callee == other.callee; }

bool EmptyNode::isEqual(const EmptyNode &other) const { return true; }
