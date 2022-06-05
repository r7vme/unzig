#include "dotgen.hpp"
#include "ast.hpp"
#include "codegen.hpp"
#include <string>
#include <sstream>

std::string getNodeId(AstNode *astNode)
{
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
  const auto text = std::string("BinExprNode\\n");
  const auto nodeId = getNodeId(astNode);
  output.append(nodeId + "[label=\"" + text + "\"]\n");
  astNode->lhs->dotgen(this, output);
  astNode->rhs->dotgen(this, output);
}

void DotGenerator::generate(VarDeclNode *astNode, std::string &output) {}
void DotGenerator::generate(FnDefNode *astNode, std::string &output) {}
void DotGenerator::generate(BlockNode *astNode, std::string &output) {}
void DotGenerator::generate(RootNode *astNode, std::string &output) {}
void DotGenerator::generate(AssignStNode *astNode, std::string &output) {}
void DotGenerator::generate(ReturnStNode *astNode, std::string &output) {}
void DotGenerator::generate(VarExprNode *astNode, std::string &output) {}
void DotGenerator::generate(FnCallExprNode *astNode, std::string &output) {}
