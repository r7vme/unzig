#pragma once

#include <string>

class FloatExprNode;
class IntegerExprNode;
class BinExprNode;
class VarDeclNode;
class FnDefNode;
class BlockNode;
class RootNode;
class AssignStNode;
class ReturnStNode;
class VarExprNode;
class FnCallExprNode;
class EmptyNode;

class DotGenerator {
  std::string output;
public:
  std::string getDotOutput();
  void generate(FloatExprNode astNode);
  void generate(IntegerExprNode astNode);
  void generate(BinExprNode astNode);
  void generate(VarDeclNode astNode);
  void generate(FnDefNode astNode);
  void generate(BlockNode astNode);
  void generate(RootNode astNode);
  void generate(AssignStNode astNode);
  void generate(ReturnStNode astNode);
  void generate(VarExprNode astNode);
  void generate(FnCallExprNode astNode);
  void generate(EmptyNode astNode);
};
