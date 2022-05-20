#pragma once

#include <llvm/IR/Constants.h>
class IntegerExprNode;
class FloatExprNode;
class BinExprNode;
class VarDeclNode;
class FnDefNode;
class BlockNode;

class AstEqualityComparator {
public:
  bool compare(const BinExprNode &lhs, const BinExprNode &rhs);
  bool compare(const VarDeclNode &lhs, const VarDeclNode &rhs);
  bool compare(const FnDefNode &lhs, const FnDefNode &rhs);
  bool compare(const BlockNode &lhs, const BlockNode &rhs);

  template <typename T>
  bool compare(const T &lhs, const T &rhs) {
    return lhs.value == rhs.value;
  }
};
