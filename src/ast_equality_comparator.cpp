#include "ast_equality_comparator.hpp"
#include "ast.hpp"

bool AstEqualityComparator::compare(const BinExprNode &lhs,
                                    const BinExprNode &rhs) {
  bool isLhsMemberEqual = lhs.lhs->isEqual(this, *(rhs.lhs));
  bool isRhsMemberEqual = lhs.rhs->isEqual(this, *(rhs.rhs));
  return lhs.type == rhs.type && isLhsMemberEqual && isRhsMemberEqual;
}

bool AstEqualityComparator::compare(const VarDeclNode &lhs,
                                    const VarDeclNode &rhs) {
  // TODO: not all fields checked
  return lhs.name == rhs.name;
}
