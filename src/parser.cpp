#include <iostream>
#include <optional>

#include "ast.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "types.hpp"

AstNodePtr parseBinOpRhsExpr(ParserCtxt &ctxt, AstNodePtr lhs);
AstNodePtr parsePrimaryExpr(ParserCtxt &ctxt);
AstNodePtr parseGroupedExpr(ParserCtxt &ctxt);
AstNodePtr parseNumberExpr(ParserCtxt &ctxt);
AstNodePtr parseExpr(ParserCtxt &ctxt);
AstNodePtr parseVarDecl(ParserCtxt &ctxt);
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

AstNodePtr parseVarDecl(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto kwVarToken = ctxt.getTokenAndAdvance();
  auto varIdentifierToken = ctxt.getTokenAndAdvance();
  auto colonToken = ctxt.getTokenAndAdvance();
  auto typeExprToken = ctxt.getTokenAndAdvance();
  if ((kwVarToken.id == TokenId::KwVar) &&
      (varIdentifierToken.id == TokenId::Identifier) &&
      (colonToken.id == TokenId::Colon) &&
      (typeExprToken.id == TokenId::Identifier)) {
    auto varName = varIdentifierToken.value;
    auto varType = toUzType(typeExprToken.value);
    if (!varType) {
      ctxt.resetCursor(mark);
      return LogError("unknown type");
    }

    // optional
    AstNodePtr initExpr = nullptr;
    auto equalToken = ctxt.getToken();
    if (equalToken.id == TokenId::Equal) {
      ctxt.skipToken();
      initExpr = parseExpr(ctxt);
      if (!initExpr) {
        ctxt.resetCursor(mark);
        return LogError("unable parse expression");
      }
    }

    auto semicolonToken = ctxt.getTokenAndAdvance();
    if (semicolonToken.id != TokenId::Semicolon) {
      ctxt.resetCursor(mark);
      return LogError("missing semicolon");
    }

    return std::make_shared<VarDeclNode>(varName, varType.value(), initExpr);
  }

  ctxt.resetCursor(mark);
  return LogError("unable to parse VarDecl");
}

AstNodePtr parseBlock(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::LBrace) {
    ctxt.resetCursor(mark);
    return LogError("unable to parse Block");
  }

  // parseStatement

  if (ctxt.getTokenAndAdvance().id != TokenId::RBrace) {
    ctxt.resetCursor(mark);
    return LogError("unable to parse Block");
  }

  return std::make_shared<BlockNode>();
}

AstNodePtr parseFnDef(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto kwFnToken = ctxt.getTokenAndAdvance();
  auto fnIdentifierToken = ctxt.getTokenAndAdvance();
  auto lParenToken = ctxt.getTokenAndAdvance();
  auto rParenToken = ctxt.getTokenAndAdvance();
  auto typeExprToken = ctxt.getTokenAndAdvance();
  if ((kwFnToken.id == TokenId::KwFn) &&
      (fnIdentifierToken.id == TokenId::Identifier) &&
      (lParenToken.id == TokenId::LParen) &&
      (rParenToken.id == TokenId::RParen) &&
      (typeExprToken.id == TokenId::Identifier)) {
    auto fnBody = parseBlock(ctxt);
    if (!fnBody) {
      ctxt.resetCursor(mark);
      return LogError("unable to parse function body");
    }

    auto fnName = fnIdentifierToken.value;
    auto fnReturnType = toUzType(typeExprToken.value);
    return std::make_shared<FnDefNode>(fnName, fnReturnType.value(), fnBody);
  }

  ctxt.resetCursor(mark);
  return LogError("unable to parse FnDef");
}

// entrypoint
AstNodePtr parse(Tokens &&tokens) {
  ParserCtxt ctxt(std::move(tokens));
  return parseExpr(ctxt);
}
