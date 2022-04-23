#include <iostream>
#include <optional>

#include "parser.hpp"

static const std::map<BinOpType, uint32_t> binOpPrec{
    {BinOpType::ADD, 10},
    {BinOpType::SUB, 10},
    {BinOpType::MUL, 20},
    {BinOpType::DIV, 20},
};

std::shared_ptr<AstNode> LogError(const std::string &str) {
  std::cerr << "error: " << str << std::endl;
  return nullptr;
}

// NumberExpr <- FLOAT / INTEGER
AstNodePtr parseNumberExpr(ParserCtxt &ctxt) {
  auto token = ctxt.getTokenAndAdvance();

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

bool isBinOp(const Token &token) {
  return (token.id == TokenId::Asterisk) || (token.id == TokenId::Colon) ||
         (token.id == TokenId::Plus) || (token.id == TokenId::Minus);
}

std::optional<BinOpType> mayBeToBinOpType(const Token &token) {
  switch (token.id) {
  case TokenId::Asterisk:
    return BinOpType::MUL;
  case TokenId::Slash:
    return BinOpType::DIV;
  case TokenId::Plus:
    return BinOpType::ADD;
  case TokenId::Minus:
    return BinOpType::SUB;
  default:
    return std::nullopt;
  }
}

// BinOpRhsExpr <- (BinOp PrimaryExpr)*
AstNodePtr parseBinOpRhsExpr(ParserCtxt &ctxt, AstNodePtr lhs) {
  while (true) {
    auto binOp = mayBeToBinOpType(ctxt.getTokenAndAdvance());
    if (!binOp) {
      return lhs;
    }

    auto rhs = parsePrimaryExpr(ctxt);
    if (!rhs) {
      return nullptr;
    }

    auto nextBinOp = mayBeToBinOpType(ctxt.getToken());
    if ((nextBinOp) &&
        (binOpPrec.at(nextBinOp.value()) > binOpPrec.at(binOp.value()))) {
      rhs = parseBinOpRhsExpr(ctxt, rhs);
    }

    lhs = std::make_shared<BinExprNode>(binOp.value(), lhs, rhs);
  }
}

// Expr <- PrimaryExpr BinOpRhsExpr
AstNodePtr parseExpr(ParserCtxt &ctxt) {
  auto lhs = parsePrimaryExpr(ctxt);
  if (!lhs) {
    return nullptr;
  }

  return parseBinOpRhsExpr(ctxt, lhs);
}

AstNodePtr parse(Tokens &&tokens) {
  ParserCtxt ctxt(std::move(tokens));
  return parseExpr(ctxt);
}
