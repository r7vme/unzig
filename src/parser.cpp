#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "ast.hpp"
#include "parser.hpp"
#include "types.hpp"

AstNode parseBinOpRhsExpr(ParserCtxt &ctxt, AstNode lhs);
AstNode parsePrimaryExpr(ParserCtxt &ctxt);
AstNode parseGroupedExpr(ParserCtxt &ctxt);
AstNode parseNumberExpr(ParserCtxt &ctxt);
AstNode parseExpr(ParserCtxt &ctxt);
AstNode parseVarDecl(ParserCtxt &ctxt);
AstNode LogError(const std::string &str);
std::optional<BinOpType> mayBeToBinOpType(const Token &token);

AstNode resetToken(ParserCtxt &ctxt, const size_t resetMark) {
  ctxt.resetCursor(resetMark);
  return EmptyNode();
}

std::string getHighlightedSourceCodeForPosition(const std::string &input,
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
      getHighlightedSourceCodeForPosition(ctxt.getSource(), token.position);
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
AstNode parseNumberExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  switch (token.id) {
  case (TokenId::IntegerLiteral):
    return IntegerExprNode(token.value);
  case (TokenId::FloatLiteral):
    return FloatExprNode(token.value);
  default:
    return resetToken(ctxt, mark);
  }
  assert(false);
}

// GroupedExpr <- LPAREN Expr RPAREN
AstNode parseGroupedExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::LParen) {
    return resetToken(ctxt, mark);
  }

  auto expr = parseExpr(ctxt);
  if (!expr) {
    fatalSyntaxError(ctxt, mark, "unable to parse grouped expression");
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RParen) {
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected )");
  }

  return expr;
}

// VarExpr <- IDENTIFIER
AstNode parseVarExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  if (token.id != TokenId::Identifier) {
    return resetToken(ctxt, mark);
  }

  return VarExprNode(token.value);
}

// FnCallExpr <- IDENTIFIER LPAREN RPAREN
AstNode parseFnCallExpr(ParserCtxt &ctxt) {
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
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected )");
  }

  return FnCallExprNode(fnNameToken.value);
}

// PrimaryExpr <- GroupedExpr
//             / FnCallExpr
//             / VarExpr
//             / NumberExpr
AstNode parsePrimaryExpr(ParserCtxt &ctxt) {
  if (auto expr = parseGroupedExpr(ctxt))
    return expr;
  if (auto expr = parseFnCallExpr(ctxt))
    return expr;
  if (auto expr = parseVarExpr(ctxt))
    return expr;
  if (auto expr = parseNumberExpr(ctxt))
    return expr;
  return EmptyNode();
}

// BinOpRhsExpr <- (BinOp PrimaryExpr)*
AstNode parseBinOpRhsExpr(ParserCtxt &ctxt, AstNode lhs) {
  while (true) {
    auto binOp = mayBeToBinOpType(ctxt.getToken());
    if (!binOp) {
      return lhs;
    }
    ctxt.skipToken();

    auto rhs = parsePrimaryExpr(ctxt);
    if (!rhs) {
      fatalSyntaxError(ctxt, ctxt.getCursor(), "expected primary expression");
    }

    auto nextBinOp = mayBeToBinOpType(ctxt.getToken());
    if ((nextBinOp) &&
        (binOpPrec.at(nextBinOp.value()) > binOpPrec.at(binOp.value()))) {
      rhs = parseBinOpRhsExpr(ctxt, rhs);
    }

    lhs = BinExprNode(binOp.value(), lhs, rhs);
  }
}

// Expr <- PrimaryExpr BinOpRhsExpr
AstNode parseExpr(ParserCtxt &ctxt) {
  if (auto lhs = parsePrimaryExpr(ctxt)) {
    return parseBinOpRhsExpr(ctxt, lhs);
  }
  return EmptyNode();
}

// VarDecl <- KEYWORD_var IDENTIFIER COLON TypeExpr (AssignOp Expr)? SEMICOLON
AstNode parseVarDecl(ParserCtxt &ctxt) {
  const std::string errorMsg = "unable to parse variable declaration";
  const auto mark = ctxt.getCursor();

  auto kwVarToken = ctxt.getTokenAndAdvance();
  if (kwVarToken.id != TokenId::KwVar)
    return resetToken(ctxt, mark);

  auto varIdentifierToken = ctxt.getTokenAndAdvance();
  if (varIdentifierToken.id != TokenId::Identifier)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto colonToken = ctxt.getTokenAndAdvance();
  if (colonToken.id != TokenId::Colon)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto typeExprToken = ctxt.getTokenAndAdvance();
  if (typeExprToken.id != TokenId::Identifier)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto varType = toUzType(typeExprToken.value);
  if (!varType)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(),
                     std::string("unknown type ") + typeExprToken.value);

  // optional
  AstNode initExpr = EmptyNode();
  auto equalToken = ctxt.getToken();
  if (equalToken.id == TokenId::Equal) {
    ctxt.skipToken();
    initExpr = parseExpr(ctxt);
    if (!initExpr) {
      fatalSyntaxError(ctxt, ctxt.getCursor(),
                       "unable to parse initialization expression");
    }
  }

  auto semicolonToken = ctxt.getTokenAndAdvance();
  if (semicolonToken.id != TokenId::Semicolon)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected semicolon");

  return VarDeclNode(varIdentifierToken.value, varType.value(),
                     initExpr);
}

// ReturnSt <- KEYWORD_return Expr? SEMICOLON
AstNode parseReturnSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::KwReturn)
    return resetToken(ctxt, mark);

  auto mayBeExpr = parseExpr(ctxt);

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon)
    fatalSyntaxError(ctxt, ctxt.getCursor(), "missing semicolon");

  return ReturnStNode(mayBeExpr);
}

// AssignSt <- Expr AssignOp Expr SEMICOLON
AstNode parseAssignSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto lhs = parseExpr(ctxt);
  if (!lhs)
    return resetToken(ctxt, mark);

  if (ctxt.getTokenAndAdvance().id != TokenId::Equal)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected right brace");

  auto rhs = parseExpr(ctxt);
  if (!rhs)
    fatalSyntaxError(ctxt, ctxt.getCursor(),
                     "unable to parse assign statement");

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "missing semicolon");

  return AssignStNode(lhs, rhs);
}

// Statement <- VarDecl
//           / AssignSt
//           / ReturnSt
AstNode parseStatement(ParserCtxt &ctxt) {
  if (auto varDecl = parseVarDecl(ctxt))
    return varDecl;
  if (auto assighSt = parseAssignSt(ctxt))
    return assighSt;
  if (auto returnSt = parseReturnSt(ctxt))
    return returnSt;
  return EmptyNode();
}

// Block <- LBRACE Statement* RBRACE
AstNode parseBlock(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  if (ctxt.getTokenAndAdvance().id != TokenId::LBrace) {
    return resetToken(ctxt, mark);
  }

  std::vector<AstNode> statements;
  while (auto statement = parseStatement(ctxt)) {
    statements.push_back(statement);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RBrace)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected right brace");

  return BlockNode(statements);
}

// FnDef <- KEYWORD_fn IDENTIFIER LPAREN RPAREN TypeExpr Block
AstNode parseFnDef(ParserCtxt &ctxt) {
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

  return FnDefNode(fnIdentifierToken.value, fnReturnType.value(),
                   fnBody);
}

// TopLevelDecl <- FnDef
//              / VarDecl
AstNode parseTopLevelDecl(ParserCtxt &ctxt) {
  if (auto fnDef = parseFnDef(ctxt))
    return fnDef;
  if (auto varDecl = parseVarDecl(ctxt))
    return varDecl;
  return EmptyNode();
}

// TopLevelDeclarations <- TopLevelDecl TopLevelDeclarations*
AstNode parseTopLevelDeclarations(ParserCtxt &ctxt) {
  auto tld = parseTopLevelDecl(ctxt);
  if (!tld)
    fatalSyntaxError(ctxt, "expected at least one top level declaration");

  std::vector<AstNode> declarations;
  declarations.push_back(tld);
  while (auto mayBeDecl = parseTopLevelDecl(ctxt)) {
    declarations.push_back(mayBeDecl);
  }
  return RootNode(declarations);
}

// Root <- skip TopLevelDeclarations eof
AstNode parseRoot(ParserCtxt &ctxt) {
  auto tlds = parseTopLevelDeclarations(ctxt);
  if (!tlds)
    fatalSyntaxError(ctxt, "unable to parse top level declarations");

  // make sure all tokens consumed
  if (ctxt.getToken().id != TokenId::Eof)
    fatalSyntaxError(ctxt, "expected eof");
  return tlds;
}

AstNode parse(const Tokens &tokens, const std::string &source) {
  ParserCtxt ctxt(tokens, source);
  if (auto root = parseRoot(ctxt)) {
    return root;
  }
  fatalSyntaxError(ctxt, "unable to parse");
  std::exit(EXIT_FAILURE);
}
