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

class DotGenerator {
public:
  void generate(FloatExprNode *astNode, std::string &output);
  void generate(IntegerExprNode *astNode, std::string &output);
  void generate(BinExprNode *astNode, std::string &output);
  void generate(VarDeclNode *astNode, std::string &output);
  void generate(FnDefNode *astNode, std::string &output);
  void generate(BlockNode *astNode, std::string &output);
  void generate(RootNode *astNode, std::string &output);
  void generate(AssignStNode *astNode, std::string &output);
  void generate(ReturnStNode *astNode, std::string &output);
  void generate(VarExprNode *astNode, std::string &output);
  void generate(FnCallExprNode *astNode, std::string &output);
};
