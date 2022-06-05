#include <iostream>
#include <optional>
#include <vector>

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
std::optional<BinOpType> mayBeToBinOpType(const Token &token);

AstNodePtr parseErr(ParserCtxt &ctxt, const size_t resetMark) {
  ctxt.resetCursor(resetMark);
  return nullptr;
}

static const std::map<BinOpType, uint32_t> binOpPrec{
    {BinOpType::ADD, 10},
    {BinOpType::SUB, 10},
    {BinOpType::MUL, 20},
    {BinOpType::DIV, 20},
};

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
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  switch (token.id) {
  case (TokenId::IntegerLiteral):
    return std::make_shared<IntegerExprNode>(token.value);
  case (TokenId::FloatLiteral):
    return std::make_shared<FloatExprNode>(token.value);
  default:
    return parseErr(ctxt, mark);
  }
  assert(false);
}

// GroupedExpr <- LPAREN Expr RPAREN
AstNodePtr parseGroupedExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::LParen) {
    return parseErr(ctxt, mark);
  }

  auto expr = parseExpr(ctxt);
  if (!expr) {
    return parseErr(ctxt, mark);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RParen) {
    return parseErr(ctxt, mark);
  }

  return expr;
}

// VarExpr <- IDENTIFIER
AstNodePtr parseVarExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  if (token.id != TokenId::Identifier) {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<VarExprNode>(token.value);
}

// FnCallExpr <- IDENTIFIER LPAREN RPAREN
AstNodePtr parseFnCallExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto fnNameToken = ctxt.getTokenAndAdvance();
  if (fnNameToken.id != TokenId::Identifier) {
    return parseErr(ctxt, mark);
  }

  auto lParenToken = ctxt.getTokenAndAdvance();
  if (lParenToken.id != TokenId::LParen) {
    return parseErr(ctxt, mark);
  }

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen) {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<FnCallExprNode>(fnNameToken.value);
}

// PrimaryExpr <- GroupedExpr
//             / FnCallExpr
//             / VarExpr
//             / NumberExpr
AstNodePtr parsePrimaryExpr(ParserCtxt &ctxt) {
  if (auto expr = parseGroupedExpr(ctxt))
    return expr;
  if (auto expr = parseFnCallExpr(ctxt))
    return expr;
  if (auto expr = parseVarExpr(ctxt))
    return expr;
  if (auto expr = parseNumberExpr(ctxt))
    return expr;
  return nullptr;
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
  if (auto lhs = parsePrimaryExpr(ctxt)) {
    return parseBinOpRhsExpr(ctxt, lhs);
  }
  return nullptr;
}

// VarDecl <- KEYWORD_var IDENTIFIER COLON TypeExpr (AssignOp Expr)? SEMICOLON
AstNodePtr parseVarDecl(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto kwVarToken = ctxt.getTokenAndAdvance();
  if (kwVarToken.id != TokenId::KwVar) {
    return parseErr(ctxt, mark);
  }

  auto varIdentifierToken = ctxt.getTokenAndAdvance();
  if (varIdentifierToken.id != TokenId::Identifier) {
    return parseErr(ctxt, mark);
  }

  auto colonToken = ctxt.getTokenAndAdvance();
  if (colonToken.id != TokenId::Colon) {
    return parseErr(ctxt, mark);
  }

  auto typeExprToken = ctxt.getTokenAndAdvance();
  if (typeExprToken.id != TokenId::Identifier) {
    return parseErr(ctxt, mark);
  }

  auto varName = varIdentifierToken.value;
  auto varType = toUzType(typeExprToken.value);
  if (!varType) {
    return parseErr(ctxt, mark);
  }

  // optional
  AstNodePtr initExpr = nullptr;
  auto equalToken = ctxt.getToken();
  if (equalToken.id == TokenId::Equal) {
    ctxt.skipToken();
    initExpr = parseExpr(ctxt);
    if (!initExpr) {
      return parseErr(ctxt, mark);
    }
  }

  auto semicolonToken = ctxt.getTokenAndAdvance();
  if (semicolonToken.id != TokenId::Semicolon) {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<VarDeclNode>(varName, varType.value(), initExpr);
}

// ReturnSt <- KEYWORD_return Expr? SEMICOLON
AstNodePtr parseReturnSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::KwReturn) {
    return parseErr(ctxt, mark);
  }

  auto mayBeExpr = parseExpr(ctxt);

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon) {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<ReturnStNode>(mayBeExpr);
}

// AssignSt <- Expr AssignOp Expr SEMICOLON
AstNodePtr parseAssignSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto lhs = parseExpr(ctxt);
  if (!lhs) {
    return parseErr(ctxt, mark);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::Equal) {
    return parseErr(ctxt, mark);
  }

  auto rhs = parseExpr(ctxt);
  if (!rhs) {
    return parseErr(ctxt, mark);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon) {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<AssignStNode>(lhs, rhs);
}

// Statement <- VarDecl
//           / AssignSt
//           / ReturnSt
AstNodePtr parseStatement(ParserCtxt &ctxt) {
  if (auto varDecl = parseVarDecl(ctxt))
    return varDecl;
  if (auto assighSt = parseAssignSt(ctxt))
    return assighSt;
  if (auto returnSt = parseReturnSt(ctxt))
    return returnSt;
  return nullptr;
}

// Block <- LBRACE Statement* RBRACE
AstNodePtr parseBlock(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::LBrace) {
    return parseErr(ctxt, mark);
  }

  std::vector<AstNodePtr> mayBeStatements;
  while (true) {
    if (auto statement = parseStatement(ctxt)) {
      mayBeStatements.push_back(statement);
    } else {
      break;
    }
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RBrace) {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<BlockNode>(mayBeStatements);
}

// FnDef <- KEYWORD_fn IDENTIFIER LPAREN RPAREN TypeExpr Block
AstNodePtr parseFnDef(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto kwFnToken = ctxt.getTokenAndAdvance();
  if (kwFnToken.id != TokenId::KwFn) {
    return parseErr(ctxt, mark);
  }

  auto fnIdentifierToken = ctxt.getTokenAndAdvance();
  if (fnIdentifierToken.id != TokenId::Identifier) {
    return parseErr(ctxt, mark);
  }

  auto lParenToken = ctxt.getTokenAndAdvance();
  if (lParenToken.id != TokenId::LParen) {
    return parseErr(ctxt, mark);
  }

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen) {
    return parseErr(ctxt, mark);
  }

  auto typeExprToken = ctxt.getTokenAndAdvance();
  if (typeExprToken.id != TokenId::Identifier) {
    return parseErr(ctxt, mark);
  }

  auto fnBody = parseBlock(ctxt);
  if (!fnBody) {
    return parseErr(ctxt, mark);
  }

  auto fnName = fnIdentifierToken.value;
  auto fnReturnType = toUzType(typeExprToken.value);
  if (!fnReturnType)
  {
    return parseErr(ctxt, mark);
  }

  return std::make_shared<FnDefNode>(fnName, fnReturnType.value(), fnBody);
}

// TopLevelDecl <- FnDef
//              / VarDecl
AstNodePtr parseTopLevelDecl(ParserCtxt &ctxt) {
  if (auto fnDef = parseFnDef(ctxt))
    return fnDef;
  if (auto varDecl = parseVarDecl(ctxt))
    return varDecl;
  return nullptr;
}

// TopLevelDeclarations <- TopLevelDecl TopLevelDeclarations*
AstNodePtr parseTopLevelDeclarations(ParserCtxt &ctxt) {
  auto topLevelDecl = parseTopLevelDecl(ctxt);
  if (!topLevelDecl) {
    return nullptr;
  }

  std::vector<AstNodePtr> declarations;
  declarations.push_back(topLevelDecl);
  while (true) {
    if (auto declaration = parseTopLevelDecl(ctxt)) {
      declarations.push_back(declaration);
    } else {
      break;
    }
  }

  return std::make_shared<RootNode>(declarations);
}

// Root <- skip TopLevelDeclarations eof
AstNodePtr parseRoot(ParserCtxt &ctxt) {
  return parseTopLevelDeclarations(ctxt);
}

AstNodePtr parse(Tokens &&tokens) {
  ParserCtxt ctxt(std::move(tokens));
  return parseRoot(ctxt);
}
