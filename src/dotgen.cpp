#include "dotgen.hpp"
#include "ast.hpp"
#include <sstream>
#include <string>

std::string getNodeId(AstNode *astNode) {
  std::ostringstream ss;
  ss << "node_" << static_cast<const void *>(astNode);
  return ss.str();
}

void DotGenerator::generate(FloatExprNode *astNode, std::string &output) {
  const auto text = std::string("FloatExpr\\n") + astNode->value;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(IntegerExprNode *astNode, std::string &output) {
  const auto text = std::string("IntegerExpr\\n") + astNode->value;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(BinExprNode *astNode, std::string &output) {
  std::string binOpType;
  switch (astNode->type) {
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
  astNode->lhs->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->lhs.get()) + "\n");

  // rhs
  astNode->rhs->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->rhs.get()) + "\n");
}

void DotGenerator::generate(VarDeclNode *astNode, std::string &output) {
  const auto text = std::string("VarDecl\\n") + astNode->name;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  if (astNode->initExpr) {
    astNode->initExpr->dotgen(this, output);
    output.append(nodeId + "->" + getNodeId(astNode->initExpr.get()) + "\n");
  }
}

void DotGenerator::generate(FnDefNode *astNode, std::string &output) {
  const auto text = std::string("FnDef\\n") + astNode->name;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  astNode->body->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->body.get()) + "\n");
}

void DotGenerator::generate(BlockNode *astNode, std::string &output) {
  const auto text = std::string("Block");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  for (auto &d : astNode->statements) {
    d->dotgen(this, output);
    output.append(nodeId + "->" + getNodeId(d.get()) + "\n");
  }
}

void DotGenerator::generate(RootNode *astNode, std::string &output) {
  const auto text = std::string("Root");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  for (auto &d : astNode->declarations) {
    d->dotgen(this, output);
    output.append(nodeId + "->" + getNodeId(d.get()) + "\n");
  }
}
void DotGenerator::generate(AssignStNode *astNode, std::string &output) {
  const auto text = std::string("AssignSt");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  // lhs
  astNode->lhs->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->lhs.get()) + "\n");

  // rhs
  astNode->rhs->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->rhs.get()) + "\n");
}

void DotGenerator::generate(ReturnStNode *astNode, std::string &output) {
  const auto text = std::string("ReturnSt");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  if (astNode->expr) {
    astNode->expr->dotgen(this, output);
    output.append(nodeId + "->" + getNodeId(astNode->expr.get()) + "\n");
  }
}

void DotGenerator::generate(VarExprNode *astNode, std::string &output) {
  const auto text = std::string("VarExpr\\n") + astNode->name;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}

void DotGenerator::generate(FnCallExprNode *astNode, std::string &output) {
  const auto text = std::string("FnCallExpr\\n") + astNode->callee;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
}
