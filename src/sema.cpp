#include "sema.hpp"
#include "ast.hpp"
#include "scope.hpp"
#include "symbol.hpp"

void SemanticAnalyzer::fatalSemaError(const std::string &msg, const size_t sourcePos) {
  auto hightlightedLine = cc->source->getHightlightedPosition(sourcePos);
  std::cerr << "Semantic error: " << msg << '\n' << hightlightedLine << std::endl;
  std::exit(EXIT_FAILURE);
}

void SemanticAnalyzer::analyze(RootNode &astNode) {
  auto globalScope = createChildScope(nullptr);
  globalScope->isGlobal = true;
  astNode.setScope(globalScope);

  for (auto &child : astNode.declarations) {
    child.setScope(globalScope);
    child.sema(this);
  }
}

void SemanticAnalyzer::analyze(VarDeclNode &astNode) {
  if (astNode.scope->lookupSymbol(astNode.name)) {
    fatalSemaError("redifinition of the symbol", astNode.sourcePos);
  }

  if (auto type = cc->typeTable.findType(astNode.typeName)) {
    astNode.type = type.value();
  } else {
    fatalSemaError(std::string("unknown return type ") + astNode.typeName, astNode.sourcePos);
  }

  astNode.symbol =
      createSymbol(SymbolType::Var, astNode.name, astNode.type, astNode.scope->isGlobal);
  astNode.scope->insertSymbol(astNode.symbol);
  astNode.initExpr.setScope(astNode.scope);
  astNode.initExpr.sema(this);
}

void SemanticAnalyzer::analyze(FnDefNode &astNode) {
  if (astNode.scope->lookupSymbol(astNode.name)) {
    fatalSemaError("redifinition of the symbol", astNode.sourcePos);
  }

  if (auto type = cc->typeTable.findType(astNode.returnTypeName)) {
    astNode.returnType = type.value();
  } else {
    fatalSemaError(std::string("unknown return type ") + astNode.returnTypeName, astNode.sourcePos);
  }

  astNode.scope->insertSymbol(
      createSymbol(SymbolType::Fn, astNode.name, astNode.returnType, astNode.scope->isGlobal));

  // new scope
  auto blockScope = createChildScope(astNode.scope);
  astNode.body.setScope(blockScope);
  astNode.body.sema(this);
}

void SemanticAnalyzer::analyze(BlockNode &astNode) {
  for (auto &statement : astNode.statements) {
    statement.setScope(astNode.scope);
    statement.sema(this);
  }
}

void SemanticAnalyzer::analyze(AssignStNode &astNode) {
  auto symbol = astNode.scope->lookupSymbol(astNode.varName);
  if (!symbol || symbol.value()->symbolType != SymbolType::Var) {
    fatalSemaError("undeclared variable", astNode.sourcePos);
  }
  astNode.varSymbol = symbol.value();
  astNode.expr.setScope(astNode.scope);
  astNode.expr.sema(this);
}

void SemanticAnalyzer::analyze(ReturnStNode &astNode) {
  astNode.expr.setScope(astNode.scope);
  astNode.expr.sema(this);
}

void SemanticAnalyzer::analyze(IfStNode &astNode) {
  astNode.condition.setScope(astNode.scope);
  astNode.condition.sema(this);

  auto blockScope = createChildScope(astNode.scope);
  astNode.block.setScope(blockScope);
  astNode.block.sema(this);

  auto elseScope = createChildScope(astNode.scope);
  astNode.elseStatement.setScope(elseScope);
  astNode.elseStatement.sema(this);
}

void SemanticAnalyzer::analyze(VarExprNode &astNode) {
  auto symbol = astNode.scope->lookupSymbol(astNode.name);
  if (!symbol || symbol.value()->symbolType != SymbolType::Var) {
    fatalSemaError("undeclared variable", astNode.sourcePos);
  }

  astNode.varSymbol = symbol.value();
}

void SemanticAnalyzer::analyze(FnCallExprNode &astNode) {
  auto symbol = astNode.scope->lookupSymbol(astNode.callee);
  if (!symbol || symbol.value()->symbolType != SymbolType::Fn) {
    fatalSemaError("undeclared function", astNode.sourcePos);
  }
}

void SemanticAnalyzer::analyze(BinExprNode &astNode) {
  astNode.lhs.setScope(astNode.scope);
  astNode.lhs.sema(this);

  astNode.rhs.setScope(astNode.scope);
  astNode.rhs.sema(this);
}

void SemanticAnalyzer::analyze(FloatExprNode &astNode) {}
void SemanticAnalyzer::analyze(IntegerExprNode &astNode) {}
void SemanticAnalyzer::analyze(EmptyNode &astNode) {}
