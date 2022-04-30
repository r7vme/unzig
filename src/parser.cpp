#include <iostream>
#include <optional>

#include "ast.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

AstNodePtr parseBinOpRhsExpr(ParserCtxt &ctxt, AstNodePtr lhs);
AstNodePtr parsePrimaryExpr(ParserCtxt &ctxt);
AstNodePtr parseGroupedExpr(ParserCtxt &ctxt);
AstNodePtr parseNumberExpr(ParserCtxt &ctxt);
AstNodePtr parseExpr(ParserCtxt &ctxt);
AstNodePtr LogError(const std::string &str);
bool isBinOp(const Token &token);
std::optional<BinOpType> mayBeToBinOpType(const Token &token);

AstNodePtr LogError(const std::string &str) {
  std::cerr << "error: " << str << std::endl;
  return nullptr;
}

static const std::map<BinOpType, uint32_t> binOpPrec{
    {BinOpType::ADD, 10},
    {BinOpType::SUB, 10},
    {BinOpType::MUL, 20},
    {BinOpType::DIV, 20},
};

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

// NumberExpr <- FLOAT / INTEGER
AstNodePtr parseNumberExpr(ParserCtxt &ctxt) {
  auto token = ctxt.getTokenAndAdvance();
  switch (token.id) {
  case (TokenId::IntegerLiteral):
    return std::make_shared<IntegerExprNode>(token.value);
  case (TokenId::FloatLiteral):
    return std::make_shared<FloatExprNode>(token.value);
  default:
    return LogError(std::string(__func__) + "unknown token");
  }
  assert(false);
}

// GroupedExpr <- LPAREN Expr RPAREN
AstNodePtr parseGroupedExpr(ParserCtxt &ctxt) {
  if (ctxt.getTokenAndAdvance().id != TokenId::LParen) {
    LogError("expected left parenthesis");
  }

  auto expr = parseExpr(ctxt);
  if (!expr) {
    return nullptr;
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RParen) {
    return LogError("expected right parenthesis");
  }

  return expr;
}

// PrimaryExpr <- GroupedExpr
//             / FnCallExpr
//             / VarExpr
//             / NumberExpr
AstNodePtr parsePrimaryExpr(ParserCtxt &ctxt) {
  // TODO: FnCallExpr / VarExpr
  switch (ctxt.getToken().id) {
  case TokenId::LParen:
    return parseGroupedExpr(ctxt);
  default:
    return parseNumberExpr(ctxt);
  }
  assert(false);
}

// BinOpRhsExpr <- (BinOp PrimaryExpr)*
AstNodePtr parseBinOpRhsExpr(ParserCtxt &ctxt, AstNodePtr lhs) {
  while (true) {
    auto binOp = mayBeToBinOpType(ctxt.getToken());
    if (!binOp) {
      return lhs;
    }
    ctxt.skipToken();

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

// entrypoint
AstNodePtr parse(Tokens &&tokens) {
  ParserCtxt ctxt(std::move(tokens));
  return parseExpr(ctxt);
}
