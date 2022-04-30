#pragma once

class IntegerExprNode;
class BinExprNode;

class AstEqualityComparator {
public:
  bool compare(const BinExprNode &lhs, const BinExprNode &rhs);

  template <typename T>
  bool compare(const T &lhs, const T &rhs) {
    return lhs.value == rhs.value;
  }
};
