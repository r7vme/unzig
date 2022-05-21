#include "ast_equality_comparator.hpp"
#include "ast.hpp"
#include <cstddef>

bool AstEqualityComparator::compare(const FnCallExprNode &lhs,
                                    const FnCallExprNode &rhs) {
  return lhs.callee == rhs.callee;
}

bool AstEqualityComparator::compare(const VarExprNode &lhs,
                                    const VarExprNode &rhs) {
  return lhs.name == rhs.name;
}

bool AstEqualityComparator::compare(const BinExprNode &lhs,
                                    const BinExprNode &rhs) {
  bool isLhsMemberEqual = lhs.lhs->isEqual(this, *(rhs.lhs));
  bool isRhsMemberEqual = lhs.rhs->isEqual(this, *(rhs.rhs));
  return lhs.type == rhs.type && isLhsMemberEqual && isRhsMemberEqual;
}

bool AstEqualityComparator::compare(const VarDeclNode &lhs,
                                    const VarDeclNode &rhs) {
  // TODO: handle nullptr
  assert(lhs.initExpr != nullptr);
  assert(rhs.initExpr != nullptr);
  return lhs.name == rhs.name && lhs.type == rhs.type &&
         lhs.initExpr->isEqual(this, *(rhs.initExpr));
}

bool AstEqualityComparator::compare(const FnDefNode &lhs,
                                    const FnDefNode &rhs) {
  assert(lhs.body != nullptr);
  assert(rhs.body != nullptr);
  return lhs.name == rhs.name && lhs.returnType == rhs.returnType &&
         lhs.body->isEqual(this, *(rhs.body));
}

bool AstEqualityComparator::compare(const BlockNode &lhs,
                                    const BlockNode &rhs) {
  if (lhs.statements.size() != rhs.statements.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.statements.size(); ++i) {
    bool isElemEqual = lhs.statements[i]->isEqual(this, *(rhs.statements[i]));
    if (!isElemEqual) {
      return false;
    }
  }
  return true;
}

bool AstEqualityComparator::compare(const RootNode &lhs, const RootNode &rhs) {
  if (lhs.declarations.size() != rhs.declarations.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.declarations.size(); ++i) {
    bool isElemEqual =
        lhs.declarations[i]->isEqual(this, *(rhs.declarations[i]));
    if (!isElemEqual) {
      return false;
    }
  }
  return true;
}

bool AstEqualityComparator::compare(const ReturnStNode &lhs,
                                    const ReturnStNode &rhs) {
  bool isLhsNullptr = lhs.expr == nullptr;
  bool isRhsNullptr = rhs.expr == nullptr;
  if (isLhsNullptr && isRhsNullptr) {
    return true;
  } else if (isLhsNullptr != isRhsNullptr) {
    return false;
  }
  return lhs.expr->isEqual(this, *(rhs.expr));
}

bool AstEqualityComparator::compare(const AssignStNode &lhs,
                                    const AssignStNode &rhs) {
  bool isLhsMemberEqual = lhs.lhs->isEqual(this, *(rhs.lhs));
  bool isRhsMemberEqual = lhs.rhs->isEqual(this, *(rhs.rhs));
  return isLhsMemberEqual && isRhsMemberEqual;
}
