#pragma once
// Create scoped symbol tables
// Fills AST with scoped symbol tables
// Do semantic checks (e.g. resolve symbols)

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

class SemanticAnalyzer {

public:
  SemanticAnalyzer();

  void *generate(FloatExprNode astNode);
  void *generate(IntegerExprNode astNode);
  void *generate(BinExprNode astNode);
  void *generate(VarDeclNode astNode);
  void *generate(FnDefNode astNode);
  void *generate(BlockNode astNode);
  void *generate(RootNode astNode);
  void *generate(AssignStNode astNode);
  void *generate(ReturnStNode astNode);
  void *generate(VarExprNode astNode);
  void *generate(FnCallExprNode astNode);
  void *generate(EmptyNode astNode);
};
