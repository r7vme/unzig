#pragma once

class IntegerExprNode;
class FloatExprNode;
class BinExprNode;
class VarDeclNode;

class AstEqualityComparator {
public:
  bool compare(const BinExprNode &lhs, const BinExprNode &rhs);
  bool compare(const VarDeclNode &lhs, const VarDeclNode &rhs);

  template <typename T>
  bool compare(const T &lhs, const T &rhs) {
    return lhs.value == rhs.value;
  }
};
