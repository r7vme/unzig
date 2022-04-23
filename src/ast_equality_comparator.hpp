#pragma once

class IntegerExprNode;
class BinExprNode;

class AstEqualityComparator {
public:
  bool compare(const IntegerExprNode &lhs, const IntegerExprNode &rhs);
  bool compare(const BinExprNode &lhs, const BinExprNode &rhs);
};
