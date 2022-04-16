#include "parser.hpp"
#include <iostream>
#include <optional>

std::shared_ptr<AstNode> LogError(const std::string &str) {
  std::cerr << "error: " << str << std::endl;
  return nullptr;
}

// NumberExpr <- FLOAT / INTEGER
AstNodePtr parseNumberExpr(ParserCtxt &ctxt) {
  auto token = ctxt.getNextToken();

  if (token.id == TokenId::IntegerLiteral) {
    return std::make_shared<IntegerExprNode>(token.value);
  };
  return LogError("can not parse token");
}

// PrimaryExpr <- GroupedExpr
//             / FnCallExpr
//             / VarExpr
//             / NumberExpr
AstNodePtr parsePrimaryExpr(ParserCtxt &ctxt) {
  // TODO: GroupedExpr / FnCallExpr / VarExpr
  return parseNumberExpr(ctxt);
}

bool isBinaryOp(const Token &token) {
  return (token.id == TokenId::Asterisk) || (token.id == TokenId::Colon) ||
         (token.id == TokenId::Plus) || (token.id == TokenId::Minus);
}

std::optional<BinaryOpType> mayBeToBinaryOpType(const Token &token) {
  switch (token.id) {
  case TokenId::Asterisk:
    return BinaryOpType::MUL;
  case TokenId::Slash:
    return BinaryOpType::DIV;
  case TokenId::Plus:
    return BinaryOpType::ADD;
  case TokenId::Minus:
    return BinaryOpType::SUB;
  default:
    return std::nullopt;
  }
}

// Expr <- PrimaryExpr (BinaryOp PrimaryExpr)*
AstNodePtr parseExpr(ParserCtxt &ctxt) {
  auto lhs = parsePrimaryExpr(ctxt);
  if (!lhs) {
    return nullptr;
  }

  auto binaryOp = mayBeToBinaryOpType(ctxt.getNextToken());
  if (!binaryOp) {
    return lhs;
  }

  auto rhs = parsePrimaryExpr(ctxt);
  if (!rhs) {
    return nullptr;
  }

  return std::make_shared<BinaryExprNode>(binaryOp.value(), lhs, rhs);
}

AstNodePtr parse(Tokens &&tokens) {
  ParserCtxt ctxtt(std::move(tokens));
  return parseExpr(ctxtt);
}
