#include <fstream>
#include <iostream>
#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "ast.hpp"
#include "ast_node.hpp"
#include "dotgen.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "tokenizer.hpp"
#include "types.hpp"

AstNode parseExpr(ParserCtxt &ctxt);
AstNode parseVarDecl(ParserCtxt &ctxt);
AstNode parseFnDef(ParserCtxt &ctxt);
AstNode parseBlock(ParserCtxt &ctxt);
AstNode parseRoot(ParserCtxt &ctxt);

TEST_CASE("BinOpRhsExpr inverted operation priority", "[parser]") {
  // 1 + 2 * 3
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::IntegerLiteral, "3", 0},
    Token{TokenId::Eof, "", 0}
  };

  auto expectedAST = BinExprNode(
    BinOpType::ADD,
    IntegerExprNode("1", 0),
    BinExprNode(
      BinOpType::MUL,
      IntegerExprNode("2", 0),
      IntegerExprNode("3", 0), 0
    ), 0
  );

  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("BinOpRhsExpr can be parsed", "[parser]") {
  // 1 * 2 + 3
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "3", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = BinExprNode(
    BinOpType::ADD,
    BinExprNode(
      BinOpType::MUL,
      IntegerExprNode("1", 0),
      IntegerExprNode("2", 0), 0
    ),
    IntegerExprNode("3", 0), 0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("GroupedExpr can be parsed", "[parser]") {
  // 3 * (2 + 1)
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "3", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::LParen, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::RParen, "", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = BinExprNode(
    BinOpType::MUL,
    IntegerExprNode("3", 0),
    BinExprNode(
      BinOpType::ADD,
      IntegerExprNode("2", 0),
      IntegerExprNode("1", 0), 0
    ), 0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("floating point numbers expr", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::FloatLiteral, "1.0", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::FloatLiteral, "2.0", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = BinExprNode(
    BinOpType::MUL,
    FloatExprNode("1.0", 0),
    FloatExprNode("2.0", 0), 0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);
}

TEST_CASE("VarDecl can be parsed", "[parser]") {
  // var y: i32 = 123;
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::KwVar, "", 0},
    Token{TokenId::Identifier, "y", 0},
    Token{TokenId::Colon, "", 0},
    Token{TokenId::Identifier, "i32", 0},
    Token{TokenId::Equal, "", 0},
    Token{TokenId::IntegerLiteral, "123", 0},
    Token{TokenId::Semicolon, "", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = VarDeclNode(
    "y",
    "i32",
    IntegerExprNode("123", 0), 0
  );
  // clang-format on

  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseVarDecl(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("FnDef", "[parser]") {
  // fn main() void {};
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::KwFn, "", 0},
    Token{TokenId::Identifier, "main", 0},
    Token{TokenId::LParen, "", 0},
    Token{TokenId::RParen, "", 0},
    Token{TokenId::Identifier, "void", 0},
    Token{TokenId::LBrace, "", 0},
    Token{TokenId::RBrace, "", 0},
    Token{TokenId::Eof, "", 0}
  };
  // clang-format on
  //
  std::vector<AstNode> declarations;
  declarations.push_back(FnDefNode("main", "void", {}, BlockNode(std::vector<AstNode>(), 0), 0));
  auto expectedAST = RootNode(declarations, 0);

  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseRoot(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("FnDef with parameters", "[parser]") {
  // fn main(a: i32) void {};
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::KwFn, "", 0},
    Token{TokenId::Identifier, "main", 0},
    Token{TokenId::LParen, "", 0},
    Token{TokenId::Identifier, "a", 0},
    Token{TokenId::Colon, "", 0},
    Token{TokenId::Identifier, "i32", 0},
    Token{TokenId::RParen, "", 0},
    Token{TokenId::Identifier, "void", 0},
    Token{TokenId::LBrace, "", 0},
    Token{TokenId::RBrace, "", 0},
    Token{TokenId::Eof, "", 0}
  };
  // clang-format on
  //
  std::vector<AstNode> declarations;
  declarations.push_back(FnDefNode("main", "void", {FnParamNode("a", "i32", 0)}, BlockNode(std::vector<AstNode>(), 0), 0));
  auto expectedAST = RootNode(declarations, 0);

  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseRoot(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("block of statements", "[parser]") {
  //
  // {
  //   var x: i32 = 1;
  //   return 1;
  // }
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::LBrace, "", 0},
    Token{TokenId::KwVar, "", 0},
    Token{TokenId::Identifier, "x", 0},
    Token{TokenId::Colon, "", 0},
    Token{TokenId::Identifier, "i32", 0},
    Token{TokenId::Equal, "", 0},
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Semicolon, "", 0},
    Token{TokenId::KwReturn, "", 0},
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Semicolon, "", 0},
    Token{TokenId::RBrace, "", 0},
    Token{TokenId::Eof, "", 0}
  };
  // clang-format on

  auto varDecl = VarDeclNode("x", "i32", IntegerExprNode("1", 0), 0);
  std::vector<AstNode> statements{
      varDecl,
      ReturnStNode(IntegerExprNode("1", 0), 0),
  };
  auto expectedAST = BlockNode(statements, 0);

  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseBlock(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("VarExpr can be parsed", "[parser]") {
  // x + 2
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::Identifier, "x", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = BinExprNode(
    BinOpType::ADD,
    VarExprNode("x", 0),
    IntegerExprNode("2", 0), 0
    );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("FnCallExpr can be parsed", "[parser]") {
  // f() + 2
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::Identifier, "f", 0},
    Token{TokenId::LParen, "", 0},
    Token{TokenId::RParen, "", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = BinExprNode(
    BinOpType::ADD,
    FnCallExprNode("f", {}, 0),
    IntegerExprNode("2", 0), 0
    );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("FnCallExpr with arguments", "[parser]") {
  // f(1, 2)
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::Identifier, "f", 0},
    Token{TokenId::LParen, "", 0},
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Comma, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::RParen, "", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = FnCallExprNode("f", {IntegerExprNode("1", 0), IntegerExprNode("2", 0)}, 0);
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("PrefixExpr", "[parser]") {
  // !2
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::ExclamationMark, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = PrefixExprNode(
      {PrefixOpType::NOT},
      IntegerExprNode("2", 0),
      0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("PrefixExpr multiple operators", "[parser]") {
  // !!2
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::ExclamationMark, "", 0},
    Token{TokenId::ExclamationMark, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = PrefixExprNode(
      {PrefixOpType::NOT, PrefixOpType::NOT},
      IntegerExprNode("2", 0),
      0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("AndExpr", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::KwAnd, "", 0},
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = AndExprNode(
      {IntegerExprNode("1", 0), IntegerExprNode("1", 0)},
      0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}

TEST_CASE("OrExpr", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::KwOr, "", 0},
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNode expectedAST = OrExprNode(
      {IntegerExprNode("1", 0), IntegerExprNode("1", 0)},
      0
  );
  // clang-format on
  ParserCtxt ctxt(inputTokens, std::make_shared<SourceObject>(std::string("")));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST == expectedAST);
}
