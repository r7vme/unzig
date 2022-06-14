#include "dotgen.hpp"
#include "ast.hpp"
#include <sstream>
#include <string>

std::string getNodeId(AstNode astNode) {
  std::ostringstream ss;
  // TODO: wrong
  ss << "node_" << static_cast<const void *>(&astNode);
  return ss.str();
}

std::string DotGenerator::getDotOutput() {
  return std::string("digraph \"AST\" {\n") + output + "}";
}

void DotGenerator::generate(FloatExprNode astNode) {
  const auto text = std::string("FloatExpr\\n") + astNode.value;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(IntegerExprNode astNode) {
  const auto text = std::string("IntegerExpr\\n") + astNode.value;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(BinExprNode astNode) {
  std::string binOpType;
  switch (astNode.type) {
  case BinOpType::ADD:
    binOpType = "+";
    break;
  case BinOpType::SUB:
    binOpType = "-";
    break;
  case BinOpType::MUL:
    binOpType = "*";
    break;
  case BinOpType::DIV:
    binOpType = "/";
    break;
  }

  const auto text = std::string("BinExpr\\n") + binOpType;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  // lhs
  astNode.lhs.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.lhs) + "\n");

  // rhs
  astNode.rhs.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.rhs) + "\n");
}

void DotGenerator::generate(VarDeclNode astNode) {
  const auto text = std::string("VarDecl\\n") + astNode.name;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  astNode.initExpr.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.initExpr) + "\n");
}

void DotGenerator::generate(FnDefNode astNode) {
  const auto text = std::string("FnDef\\n") + astNode.name;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  astNode.body.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.body) + "\n");
}

void DotGenerator::generate(BlockNode astNode) {
  const auto text = std::string("Block");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  for (auto &d : astNode.statements) {
    d.dotgen(this);
    output.append(nodeId + "->" + getNodeId(d) + "\n");
  }
}

void DotGenerator::generate(RootNode astNode) {
  const auto text = std::string("Root");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  for (auto &d : astNode.declarations) {
    d.dotgen(this);
    output.append(nodeId + "->" + getNodeId(d) + "\n");
  }
}
void DotGenerator::generate(AssignStNode astNode) {
  const auto text = std::string("AssignSt");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  // lhs
  astNode.lhs.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.lhs) + "\n");

  // rhs
  astNode.rhs.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.rhs) + "\n");
}

void DotGenerator::generate(ReturnStNode astNode) {
  const auto text = std::string("ReturnSt");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  astNode.expr.dotgen(this);
  output.append(nodeId + "->" + getNodeId(astNode.expr) + "\n");
}

void DotGenerator::generate(VarExprNode astNode) {
  const auto text = std::string("VarExpr\\n") + astNode.name;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(FnCallExprNode astNode) {
  const auto text = std::string("FnCallExpr\\n") + astNode.callee;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(EmptyNode astNode) {
  const auto text = std::string("Empty\\n");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}
