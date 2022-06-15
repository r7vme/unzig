#include "sema.hpp"
#include "ast.hpp"
#include "scope.hpp"

void SemanticAnalyzer::fatalSemaError(const std::string & msg) {
  std::cerr << msg << std::endl;
  std::exit(EXIT_FAILURE);
}

void SemanticAnalyzer::analyze(RootNode &astNode) {
  Scope globalScope = astNode.scope;
  for (auto &child : astNode.declarations) {
    child.setScope(globalScope);
    child.sema(this);
  }
}

void SemanticAnalyzer::analyze(VarDeclNode &astNode) {
  if (astNode.scope->lookupSymbol(astNode.name)) {
    fatalSemaError("redifinition of the symbol");
  }
  astNode.scope->insertSymbol(Symbol(astNode.name, SymbolType::Var));

  astNode.initExpr.getScope()->setParent(astNode.scope);
  astNode.initExpr.sema(this);
}

void SemanticAnalyzer::analyze(FnDefNode &astNode) {
  if (astNode.scope->lookupSymbol(astNode.name)) {
    fatalSemaError("redifinition of the symbol");
  }
  astNode.scope->insertSymbol(Symbol(astNode.name, SymbolType::Fn));

  astNode.body.getScope()->setParent(astNode.scope);
  astNode.body.sema(this);
}

void SemanticAnalyzer::analyze(FloatExprNode &astNode) {}
void SemanticAnalyzer::analyze(IntegerExprNode &astNode) {}
void SemanticAnalyzer::analyze(BinExprNode &astNode) {}
void SemanticAnalyzer::analyze(BlockNode &astNode) {}
void SemanticAnalyzer::analyze(AssignStNode &astNode) {}
void SemanticAnalyzer::analyze(ReturnStNode &astNode) {}
void SemanticAnalyzer::analyze(VarExprNode &astNode) {}
void SemanticAnalyzer::analyze(FnCallExprNode &astNode) {}
void SemanticAnalyzer::analyze(EmptyNode &astNode) {}
