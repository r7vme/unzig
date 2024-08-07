#include "sema.hpp"

#include <string>
#include <iostream>

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
    fatalSemaError(std::string("unknown type ") + astNode.typeName, astNode.sourcePos);
  }

  astNode.symbol =
      createSymbol(SymbolType::Var, astNode.name, astNode.type, astNode.scope->isGlobal, 0);
  astNode.scope->insertSymbol(astNode.symbol);
  astNode.initExpr.setScope(astNode.scope);
  astNode.initExpr.sema(this);
}

void SemanticAnalyzer::analyze(FnParamNode &astNode) {
  if (astNode.scope->lookupSymbol(astNode.name)) {
    fatalSemaError("redifinition of the symbol", astNode.sourcePos);
  }

  if (auto type = cc->typeTable.findType(astNode.typeName)) {
    astNode.type = type.value();
  } else {
    fatalSemaError(std::string("unknown type ") + astNode.typeName, astNode.sourcePos);
  }

  astNode.symbol =
      createSymbol(SymbolType::Var, astNode.name, astNode.type, astNode.scope->isGlobal, 0);
  astNode.scope->insertSymbol(astNode.symbol);
}

void SemanticAnalyzer::analyze(FnDefNode &astNode) {
  if (astNode.scope->lookupSymbol(astNode.name)) {
    fatalSemaError("redifinition of the symbol", astNode.sourcePos);
  }

  if (auto type = cc->typeTable.findType(astNode.returnTypeName)) {
    astNode.returnType = type.value();
  } else {
    fatalSemaError(std::string("unknown type ") + astNode.returnTypeName, astNode.sourcePos);
  }

  astNode.scope->insertSymbol(createSymbol(SymbolType::Fn, astNode.name, astNode.returnType,
                                           astNode.scope->isGlobal, astNode.parameters.size()));

  // new scope
  auto blockScope = createChildScope(astNode.scope);

  for (auto &p : astNode.parameters) {
    p.setScope(blockScope);
    p.sema(this);
  }

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
  astNode.ifCondition.setScope(astNode.scope);
  astNode.ifCondition.sema(this);

  auto blockScope = createChildScope(astNode.scope);
  astNode.thenBlock.setScope(blockScope);
  astNode.thenBlock.sema(this);

  auto elseScope = createChildScope(astNode.scope);
  astNode.elseBlock.setScope(elseScope);
  astNode.elseBlock.sema(this);
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

  if (symbol.value()->param_num != astNode.arguments.size()) {
    fatalSemaError(std::string("wrong number of arguments. Expected ") +
                       std::to_string(symbol.value()->param_num),
                   astNode.sourcePos);
  }
}

void SemanticAnalyzer::analyze(BinExprNode &astNode) {
  astNode.lhs.setScope(astNode.scope);
  astNode.lhs.sema(this);

  astNode.rhs.setScope(astNode.scope);
  astNode.rhs.sema(this);
}

void SemanticAnalyzer::analyze(PrefixExprNode &astNode) {
  astNode.expr.setScope(astNode.scope);
  astNode.expr.sema(this);
}

void SemanticAnalyzer::analyze(OrExprNode &astNode) {
  for (auto &expr : astNode.expressions) {
    expr.setScope(astNode.scope);
    expr.sema(this);
  }
}

void SemanticAnalyzer::analyze(AndExprNode &astNode) {
  for (auto &expr : astNode.expressions) {
    expr.setScope(astNode.scope);
    expr.sema(this);
  }
}

void SemanticAnalyzer::analyze(FloatExprNode &astNode) {}
void SemanticAnalyzer::analyze(IntegerExprNode &astNode) {}
void SemanticAnalyzer::analyze(BoolExprNode &astNode) {}
void SemanticAnalyzer::analyze(EmptyNode &astNode) {}
