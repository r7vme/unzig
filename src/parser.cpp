#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
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

AstNodePtr resetToken(ParserCtxt &ctxt, const size_t resetMark) {
  ctxt.resetCursor(resetMark);
  return nullptr;
}

std::string getHighlightedSourceCode(const std::string &input,
                                     const size_t position) {
  std::size_t lineBegin = input.rfind('\n', position);
  if (lineBegin == std::string::npos)
    lineBegin = 0;

  std::size_t lineEnd = input.find('\n', position);
  if (lineEnd == std::string::npos)
    lineEnd = (input.size() - 1);

  auto line = input.substr(lineBegin, lineEnd - lineBegin);
  auto relativeTokenPosition = position - lineBegin;
  std::string highlightLine(line.size(), ' ');
  highlightLine.at(relativeTokenPosition) = '^';
  const std::string prefix = " | ";
  return prefix + line + '\n' + prefix + highlightLine;
}

void printSyntaxError(ParserCtxt &ctxt, const std::string &msg) {
  const auto token = ctxt.getToken();
  auto hightlightedLine =
      getHighlightedSourceCode(ctxt.getSource(), token.position);
  std::cerr << "Syntax error: " << msg << '\n' << hightlightedLine << std::endl;
}

void fatalSyntaxError(ParserCtxt &ctxt, const size_t resetMark,
                      const std::string &msg) {
  ctxt.resetCursor(resetMark);
  printSyntaxError(ctxt, msg);
  std::exit(EXIT_FAILURE);
}

void fatalSyntaxError(ParserCtxt &ctxt, const std::string &msg) {
  fatalSyntaxError(ctxt, ctxt.getCursor(), msg);
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
    return resetToken(ctxt, mark);
  }
  assert(false);
}

// GroupedExpr <- LPAREN Expr RPAREN
AstNodePtr parseGroupedExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::LParen) {
    return resetToken(ctxt, mark);
  }

  auto expr = parseExpr(ctxt);
  if (!expr) {
    return resetToken(ctxt, mark);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RParen) {
    return resetToken(ctxt, mark);
  }

  return expr;
}

// VarExpr <- IDENTIFIER
AstNodePtr parseVarExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  if (token.id != TokenId::Identifier) {
    return resetToken(ctxt, mark);
  }

  return std::make_shared<VarExprNode>(token.value);
}

// FnCallExpr <- IDENTIFIER LPAREN RPAREN
AstNodePtr parseFnCallExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto fnNameToken = ctxt.getTokenAndAdvance();
  if (fnNameToken.id != TokenId::Identifier) {
    return resetToken(ctxt, mark);
  }

  auto lParenToken = ctxt.getTokenAndAdvance();
  if (lParenToken.id != TokenId::LParen) {
    return resetToken(ctxt, mark);
  }

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen) {
    return resetToken(ctxt, mark);
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
    return resetToken(ctxt, mark);
  }

  auto varIdentifierToken = ctxt.getTokenAndAdvance();
  if (varIdentifierToken.id != TokenId::Identifier) {
    return resetToken(ctxt, mark);
  }

  auto colonToken = ctxt.getTokenAndAdvance();
  if (colonToken.id != TokenId::Colon) {
    return resetToken(ctxt, mark);
  }

  auto typeExprToken = ctxt.getTokenAndAdvance();
  if (typeExprToken.id != TokenId::Identifier) {
    return resetToken(ctxt, mark);
  }

  auto varName = varIdentifierToken.value;
  auto varType = toUzType(typeExprToken.value);
  if (!varType) {
    return resetToken(ctxt, mark);
  }

  // optional
  AstNodePtr initExpr = nullptr;
  auto equalToken = ctxt.getToken();
  if (equalToken.id == TokenId::Equal) {
    ctxt.skipToken();
    initExpr = parseExpr(ctxt);
    if (!initExpr) {
      return resetToken(ctxt, mark);
    }
  }

  auto semicolonToken = ctxt.getTokenAndAdvance();
  if (semicolonToken.id != TokenId::Semicolon) {
    return resetToken(ctxt, mark);
  }

  return std::make_shared<VarDeclNode>(varName, varType.value(), initExpr);
}

// ReturnSt <- KEYWORD_return Expr? SEMICOLON
AstNodePtr parseReturnSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::KwReturn) {
    return resetToken(ctxt, mark);
  }

  auto mayBeExpr = parseExpr(ctxt);

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon) {
    return resetToken(ctxt, mark);
  }

  return std::make_shared<ReturnStNode>(mayBeExpr);
}

// AssignSt <- Expr AssignOp Expr SEMICOLON
AstNodePtr parseAssignSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto lhs = parseExpr(ctxt);
  if (!lhs) {
    return resetToken(ctxt, mark);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::Equal) {
    return resetToken(ctxt, mark);
  }

  auto rhs = parseExpr(ctxt);
  if (!rhs) {
    return resetToken(ctxt, mark);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon) {
    return resetToken(ctxt, mark);
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
    return resetToken(ctxt, mark);
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
    return resetToken(ctxt, mark);
  }

  return std::make_shared<BlockNode>(mayBeStatements);
}

// FnDef <- KEYWORD_fn IDENTIFIER LPAREN RPAREN TypeExpr Block
AstNodePtr parseFnDef(ParserCtxt &ctxt) {
  const std::string errorMsg = "unable to parse function definition";
  const auto mark = ctxt.getCursor();

  auto kwFnToken = ctxt.getTokenAndAdvance();
  if (kwFnToken.id != TokenId::KwFn)
    return resetToken(ctxt, mark);

  auto fnIdentifierToken = ctxt.getTokenAndAdvance();
  if (fnIdentifierToken.id != TokenId::Identifier)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto lParenToken = ctxt.getTokenAndAdvance();
  if (lParenToken.id != TokenId::LParen)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto typeExprToken = ctxt.getTokenAndAdvance();
  if (typeExprToken.id != TokenId::Identifier)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto fnReturnType = toUzType(typeExprToken.value);
  if (!fnReturnType)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(),
                     std::string("unknown type ") + typeExprToken.value);

  auto fnBody = parseBlock(ctxt);
  if (!fnBody)
    fatalSyntaxError(ctxt, ctxt.getCursor(), errorMsg);

  return std::make_shared<FnDefNode>(fnIdentifierToken.value,
                                     fnReturnType.value(), fnBody);
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
  if (!topLevelDecl)
    fatalSyntaxError(ctxt, "expected at least one top level declaration");

  std::vector<AstNodePtr> declarations;
  declarations.push_back(topLevelDecl);
  while (auto declaration = parseTopLevelDecl(ctxt)) {
    declarations.push_back(declaration);
  }
  return std::make_shared<RootNode>(declarations);
}

// Root <- skip TopLevelDeclarations eof
AstNodePtr parseRoot(ParserCtxt &ctxt) {
  auto tlds = parseTopLevelDeclarations(ctxt);
  if (!tlds)
    fatalSyntaxError(ctxt, "unable to parse top level declarations");

  // make sure all tokens consumed
  if (ctxt.getToken().id != TokenId::Eof)
    fatalSyntaxError(ctxt, "expected eof");
  return tlds;
}

AstNodePtr parse(const Tokens &tokens, const std::string &source) {
  ParserCtxt ctxt(tokens, source);
  return parseRoot(ctxt);
}
