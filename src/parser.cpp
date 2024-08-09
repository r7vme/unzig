#include "parser.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "ast.hpp"
#include "tokenizer.hpp"

AstNode parseStatement(ParserCtxt &ctxt);
AstNode parseBlock(ParserCtxt &ctxt);
AstNode parseOrExpr(ParserCtxt &ctxt);
AstNode parseAndExpr(ParserCtxt &ctxt);
AstNode parseCompareExpr(ParserCtxt &ctxt);
AstNode parseBinaryExpr(ParserCtxt &ctxt);
AstNode parsePrefixExpr(ParserCtxt &ctxt);
AstNode parseBinOpRhsExpr(ParserCtxt &ctxt, AstNode lhs);
AstNode parsePrimaryExpr(ParserCtxt &ctxt);
AstNode parseGroupedExpr(ParserCtxt &ctxt);
AstNode parseBoolExpr(ParserCtxt &ctxt);
AstNode parseNumberExpr(ParserCtxt &ctxt);
AstNode parseExpr(ParserCtxt &ctxt);
AstNode parseVarDecl(ParserCtxt &ctxt);
AstNode parseFnDef(ParserCtxt &ctxt);
AstNode parseFnCallExpr(ParserCtxt &ctxt);
AstNode parseFnParam(ParserCtxt &ctxt);
std::optional<BinOpType> maybeToBinOpType(const Token &token);
std::optional<PrefixOpType> maybeToPrefixOpType(const Token &token);

AstNode resetToken(ParserCtxt &ctxt, const size_t resetMark) {
  ctxt.resetCursor(resetMark);
  return EmptyNode();
}

void printSyntaxError(ParserCtxt &ctxt, const std::string &msg) {
  const auto token = ctxt.getToken();
  auto hightlightedLine = ctxt.getSource()->getHightlightedPosition(token.position);
  std::cerr << "Syntax error: " << msg << '\n' << hightlightedLine << std::endl;
}

void fatalSyntaxError(ParserCtxt &ctxt, const size_t resetMark, const std::string &msg) {
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

std::optional<BinOpType> maybeToBinOpType(const Token &token) {
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

std::optional<PrefixOpType> maybeToPrefixOpType(const Token &token) {
  switch (token.id) {
  case TokenId::ExclamationMark:
    return PrefixOpType::NOT;
  case TokenId::Minus:
    return PrefixOpType::MINUS;
  default:
    return std::nullopt;
  }
}

// BoolExpr <- KEYWORD_true
//          / KEYWORD_false
AstNode parseBoolExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  switch (token.id) {
  case (TokenId::KwTrue):
    return BoolExprNode(true, token.position);
  case (TokenId::KwFalse):
    return BoolExprNode(false, token.position);
  default:
    return resetToken(ctxt, mark);
  }
  assert(false);
}

// NumberExpr <- FLOAT / INTEGER
AstNode parseNumberExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto token = ctxt.getTokenAndAdvance();
  switch (token.id) {
  case (TokenId::IntegerLiteral):
    return IntegerExprNode(token.value, token.position);
  case (TokenId::FloatLiteral):
    return FloatExprNode(token.value, token.position);
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

  return VarExprNode(token.value, token.position);
}

// FnCallExpr <- IDENTIFIER LPAREN ExprList RPAREN
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

  // ExprList
  std::vector<AstNode> arguments;
  {
    if (auto firstArg = parseExpr(ctxt)) {
      arguments.push_back(firstArg);
      while (ctxt.getToken().id == TokenId::Comma) {
        const auto commaMark = ctxt.getCursor();
        ctxt.skipToken();
        if (auto param = parseExpr(ctxt)) {
          arguments.push_back(param);
        } else {
          fatalSyntaxError(ctxt, commaMark, "unable to parse function call");
        }
      }
    }
  }

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen) {
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected )");
  }

  return FnCallExprNode(fnNameToken.value, arguments, fnNameToken.position);
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
  if (auto expr = parseBoolExpr(ctxt))
    return expr;
  if (auto expr = parseNumberExpr(ctxt))
    return expr;
  return EmptyNode();
}

// BinOpRhsExpr <- (BinOp PrefixExpr)*
AstNode parseBinOpRhsExpr(ParserCtxt &ctxt, AstNode lhs) {
  while (true) {
    auto binOpToken = ctxt.getToken();
    auto binOp = maybeToBinOpType(binOpToken);
    if (!binOp) {
      return lhs;
    }
    ctxt.skipToken();

    auto rhs = parsePrefixExpr(ctxt);
    if (!rhs) {
      fatalSyntaxError(ctxt, ctxt.getCursor(), "expected expression");
    }

    auto nextBinOp = maybeToBinOpType(ctxt.getToken());
    if ((nextBinOp) && (binOpPrec.at(nextBinOp.value()) > binOpPrec.at(binOp.value()))) {
      rhs = parseBinOpRhsExpr(ctxt, rhs);
    }

    lhs = BinExprNode(binOp.value(), lhs, rhs, binOpToken.position);
  }
}

// BoolOrExpr <- BoolAndExpr (KEYWORD_or BoolAndExpr)*
AstNode parseOrExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();
  size_t tokenPos = ctxt.getToken().position;

  auto firstExpr = parseAndExpr(ctxt);
  if (!firstExpr) {
    return resetToken(ctxt, mark);
  }

  std::vector<AstNode> expressions;
  expressions.push_back(firstExpr);
  while (ctxt.getToken().id == TokenId::KwOr) {
    ctxt.skipToken();
    if (auto expr = parseAndExpr(ctxt)) {
      expressions.push_back(expr);
    } else {
      return resetToken(ctxt, mark);
    }
  }

  if (expressions.size() == 1) {
    return firstExpr;
  }

  return OrExprNode(expressions, tokenPos);
}

// BoolAndExpr <- CompareExpr (KEYWORD_and CompareExpr)*
AstNode parseAndExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();
  size_t tokenPos = ctxt.getToken().position;

  auto firstExpr = parseCompareExpr(ctxt);
  if (!firstExpr) {
    return resetToken(ctxt, mark);
  }

  std::vector<AstNode> expressions;
  expressions.push_back(firstExpr);
  while (ctxt.getToken().id == TokenId::KwAnd) {
    ctxt.skipToken();
    if (auto expr = parseCompareExpr(ctxt)) {
      expressions.push_back(expr);
    } else {
      return resetToken(ctxt, mark);
    }
  }

  if (expressions.size() == 1) {
    return firstExpr;
  }

  return AndExprNode(expressions, tokenPos);
}

// CompareExpr <- BinaryExpr (CompareOp BinaryExpr)?
AstNode parseCompareExpr(ParserCtxt &ctxt) { return parseBinaryExpr(ctxt); }

// BinaryExpr <- PrefixExpr BinOpRhsExpr
AstNode parseBinaryExpr(ParserCtxt &ctxt) {
  if (auto lhs = parsePrefixExpr(ctxt)) {
    return parseBinOpRhsExpr(ctxt, lhs);
  }
  return EmptyNode();
}

// PrefixExpr <- PrefixOp* PrimaryExpr
AstNode parsePrefixExpr(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  size_t tokenPos = ctxt.getToken().position;
  std::vector<PrefixOpType> operators;
  while (auto prefixOp = maybeToPrefixOpType(ctxt.getToken())) {
    operators.push_back(prefixOp.value());
    ctxt.skipToken();
  }

  auto expr = parsePrimaryExpr(ctxt);
  if (!expr) {
    return resetToken(ctxt, mark);
  }

  if (operators.size() == 0) {
    return expr;
  }

  return PrefixExprNode(operators, expr, tokenPos);
}

// Expr <- BoolOrExpr
AstNode parseExpr(ParserCtxt &ctxt) { return parseOrExpr(ctxt); }

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

  // optional
  AstNode initExpr = EmptyNode();
  auto equalToken = ctxt.getToken();
  if (equalToken.id == TokenId::Equal) {
    ctxt.skipToken();
    initExpr = parseExpr(ctxt);
    if (!initExpr) {
      fatalSyntaxError(ctxt, ctxt.getCursor(), "unable to parse initialization expression");
    }
  }

  auto semicolonToken = ctxt.getTokenAndAdvance();
  if (semicolonToken.id != TokenId::Semicolon)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected semicolon");

  return VarDeclNode(varIdentifierToken.value, typeExprToken.value, initExpr, kwVarToken.position);
}

// IfSt <- KEYWORD_if LPAREN Expr RPAREN Block ( KEYWORD_else Statement )?
AstNode parseIfSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto kwIfToken = ctxt.getTokenAndAdvance();
  if (kwIfToken.id != TokenId::KwIf)
    return resetToken(ctxt, mark);

  const std::string errMsg = "unable to parse if statement";
  auto lParenToken = ctxt.getTokenAndAdvance();
  if (lParenToken.id != TokenId::LParen)
    fatalSyntaxError(ctxt, mark, errMsg);

  auto condition = parseExpr(ctxt);
  if (!condition)
    fatalSyntaxError(ctxt, mark, errMsg);

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen)
    fatalSyntaxError(ctxt, mark, errMsg);

  auto block = parseBlock(ctxt);
  if (!block)
    fatalSyntaxError(ctxt, mark, errMsg);

  AstNode elseStatement = EmptyNode();

  auto kwElseToken = ctxt.getToken();
  if (kwElseToken.id == TokenId::KwElse) {
    ctxt.skipToken();
    if (auto block = parseStatement(ctxt)) {
      elseStatement = block;
    } else {
      fatalSyntaxError(ctxt, mark, errMsg);
    }
  }

  return IfStNode(condition, block, elseStatement, kwIfToken.position);
}

// ReturnSt <- KEYWORD_return Expr? SEMICOLON
AstNode parseReturnSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto kwReturnToken = ctxt.getTokenAndAdvance();
  if (kwReturnToken.id != TokenId::KwReturn)
    return resetToken(ctxt, mark);

  auto mayBeExpr = parseExpr(ctxt);

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon)
    fatalSyntaxError(ctxt, ctxt.getCursor(), "missing semicolon");

  return ReturnStNode(mayBeExpr, kwReturnToken.position);
}

// AssignSt <- IDENTIFIER AssignOp Expr SEMICOLON
AstNode parseAssignSt(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto varNameToken = ctxt.getTokenAndAdvance();
  if (varNameToken.id != TokenId::Identifier)
    return resetToken(ctxt, mark);

  if (ctxt.getTokenAndAdvance().id != TokenId::Equal)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected equal");

  auto expr = parseExpr(ctxt);
  if (!expr)
    fatalSyntaxError(ctxt, ctxt.getCursor(), "unable to parse assign statement");

  if (ctxt.getTokenAndAdvance().id != TokenId::Semicolon)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "missing semicolon");

  return AssignStNode(varNameToken.value, expr, varNameToken.position);
}

// Statement <- VarDecl
//           / IfSt
//           / AssignSt
//           / ReturnSt
//           / Block
AstNode parseStatement(ParserCtxt &ctxt) {
  if (auto varDecl = parseVarDecl(ctxt))
    return varDecl;
  if (auto ifSt = parseIfSt(ctxt))
    return ifSt;
  if (auto assighSt = parseAssignSt(ctxt))
    return assighSt;
  if (auto returnSt = parseReturnSt(ctxt))
    return returnSt;
  if (auto block = parseBlock(ctxt))
    return block;
  return EmptyNode();
}

// Block <- LBRACE Statement* RBRACE
AstNode parseBlock(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto lBraceToken = ctxt.getTokenAndAdvance();
  if (lBraceToken.id != TokenId::LBrace) {
    return resetToken(ctxt, mark);
  }

  std::vector<AstNode> statements;
  while (auto statement = parseStatement(ctxt)) {
    statements.push_back(statement);
  }

  if (ctxt.getTokenAndAdvance().id != TokenId::RBrace)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), "expected right brace");

  return BlockNode(statements, lBraceToken.position);
}

// FnParam <- IDENTIFIER COLON TypeExpr
AstNode parseFnParam(ParserCtxt &ctxt) {
  const auto mark = ctxt.getCursor();

  auto nameToken = ctxt.getTokenAndAdvance();
  if (nameToken.id != TokenId::Identifier)
    return resetToken(ctxt, mark);

  if (ctxt.getTokenAndAdvance().id != TokenId::Colon)
    return resetToken(ctxt, mark);

  auto typeToken = ctxt.getTokenAndAdvance();
  if (typeToken.id != TokenId::Identifier)
    return resetToken(ctxt, mark);

  return FnParamNode(nameToken.value, typeToken.value, nameToken.position);
}

// FnDef <- KEYWORD_fn IDENTIFIER LPAREN FnParamList RPAREN TypeExpr Block
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

  std::vector<AstNode> parameters;
  {
    if (auto firstParam = parseFnParam(ctxt)) {
      parameters.push_back(firstParam);
      while (ctxt.getToken().id == TokenId::Comma) {
        const auto commaMark = ctxt.getCursor();
        ctxt.skipToken();
        if (auto param = parseFnParam(ctxt)) {
          parameters.push_back(param);
        } else {
          fatalSyntaxError(ctxt, commaMark, errorMsg);
        }
      }
    }
  }

  auto rParenToken = ctxt.getTokenAndAdvance();
  if (rParenToken.id != TokenId::RParen)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto typeExprToken = ctxt.getTokenAndAdvance();
  if (typeExprToken.id != TokenId::Identifier)
    fatalSyntaxError(ctxt, ctxt.getPrevCursor(), errorMsg);

  auto fnBody = parseBlock(ctxt);
  if (!fnBody)
    fatalSyntaxError(ctxt, ctxt.getCursor(), errorMsg);

  return FnDefNode(fnIdentifierToken.value, typeExprToken.value, parameters, fnBody,
                   kwFnToken.position);
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
  return RootNode(declarations, 0U);
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

AstNode parse(const Tokens &tokens, const Source &source) {
  ParserCtxt ctxt(tokens, source);
  if (auto root = parseRoot(ctxt)) {
    return root;
  }
  fatalSyntaxError(ctxt, "unable to parse");
  std::exit(EXIT_FAILURE);
}
