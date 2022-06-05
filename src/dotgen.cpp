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
  switch (astNode->type)
  {
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

  const auto text = std::string("BinExprNode\\n") + binOpType;
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");

  // lhs
  astNode->lhs->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->lhs.get()) + "\n");

  // rhs
  astNode->rhs->dotgen(this, output);
  output.append(nodeId + "->" + getNodeId(astNode->rhs.get()) + "\n");
}

void DotGenerator::generate(VarDeclNode *astNode, std::string &output) {}
void DotGenerator::generate(FnDefNode *astNode, std::string &output) {}
void DotGenerator::generate(BlockNode *astNode, std::string &output) {}
void DotGenerator::generate(RootNode *astNode, std::string &output) {}
void DotGenerator::generate(AssignStNode *astNode, std::string &output) {}
void DotGenerator::generate(ReturnStNode *astNode, std::string &output) {}
void DotGenerator::generate(VarExprNode *astNode, std::string &output) {}
void DotGenerator::generate(FnCallExprNode *astNode, std::string &output) {}
