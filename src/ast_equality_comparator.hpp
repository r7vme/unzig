#pragma once

#include <llvm/IR/Constants.h>

class IntegerExprNode;
class FloatExprNode;
class BinExprNode;
class VarDeclNode;
class FnDefNode;
class BlockNode;
class RootNode;
class AssignStNode;
class ReturnStNode;

class AstEqualityComparator {
public:
  bool compare(const BinExprNode &lhs, const BinExprNode &rhs);
  bool compare(const VarDeclNode &lhs, const VarDeclNode &rhs);
  bool compare(const FnDefNode &lhs, const FnDefNode &rhs);
  bool compare(const BlockNode &lhs, const BlockNode &rhs);
  bool compare(const RootNode &lhs, const RootNode &rhs);
  bool compare(const AssignStNode &lhs, const AssignStNode &rhs);
  bool compare(const ReturnStNode &lhs, const ReturnStNode &rhs);

  template <typename T> bool compare(const T &lhs, const T &rhs) {
    return lhs.value == rhs.value;
  }
};
