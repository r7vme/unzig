#include <iostream>
#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "ast.hpp"
#include "ast_equality_comparator.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "types.hpp"

AstNodePtr parseExpr(ParserCtxt &ctxt);
AstNodePtr parseVarDecl(ParserCtxt &ctxt);
AstNodePtr parseFnDef(ParserCtxt &ctxt);
AstNodePtr parseBlock(ParserCtxt &ctxt);

TEST_CASE("BinOpRhsExpr inverted operation priority - 1 + 2 * 3", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::IntegerLiteral, "3", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::ADD,
    std::make_shared<IntegerExprNode>("1"),
    std::make_shared<BinExprNode>(
      BinOpType::MUL,
      std::make_shared<IntegerExprNode>("2"),
      std::make_shared<IntegerExprNode>("3")
    )
  );
  // clang-format on
  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("BinOpRhsExpr - 1 * 2 + 3", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "3", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::ADD,
    std::make_shared<BinExprNode>(
      BinOpType::MUL,
      std::make_shared<IntegerExprNode>("1"),
      std::make_shared<IntegerExprNode>("2")
    ),
    std::make_shared<IntegerExprNode>("3")
  );
  // clang-format on
  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("GroupedExpr - 3 * (2 + 1)", "[parser]") {
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

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::MUL,
    std::make_shared<IntegerExprNode>("3"),
    std::make_shared<BinExprNode>(
      BinOpType::ADD,
      std::make_shared<IntegerExprNode>("2"),
      std::make_shared<IntegerExprNode>("1")
    )
  );
  // clang-format on
  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("floating point numbers expr", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::FloatLiteral, "1.0", 0},
    Token{TokenId::Asterisk, "", 0},
    Token{TokenId::FloatLiteral, "2.0", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::MUL,
    std::make_shared<FloatExprNode>("1.0"),
    std::make_shared<FloatExprNode>("2.0")
  );
  // clang-format on
  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("VarDecl 'var y: i32 = 123;'", "[parser]") {
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

  AstNodePtr expectedAST = std::make_shared<VarDeclNode>(
    "y",
    UzType{UzTypeId::Int32},
    std::make_shared<IntegerExprNode>("123")
  );
  // clang-format on

  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseVarDecl(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("FnDef 'fn main() void {};'", "[parser]") {
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
  std::vector<AstNodePtr> declarations;
  declarations.push_back(std::make_shared<FnDefNode>(
      "main", UzType{UzTypeId::Void},
      std::make_shared<BlockNode>(std::vector<AstNodePtr>())));
  auto expectedAST = std::make_shared<RootNode>(declarations);

  const std::string input{};
  auto AST = parse(inputTokens, input);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
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

  auto varDecl = std::make_shared<VarDeclNode>(
      "x", UzType{UzTypeId::Int32}, std::make_shared<IntegerExprNode>("1"));
  std::vector<AstNodePtr> statements{
      varDecl,
      std::make_shared<ReturnStNode>(std::make_shared<IntegerExprNode>("1")),
  };
  auto expected = std::make_shared<BlockNode>(statements);

  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseBlock(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expected));
}

TEST_CASE("VarExpr 'x + 2'", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::Identifier, "x", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::ADD,
    std::make_shared<VarExprNode>("x"),
    std::make_shared<IntegerExprNode>("2")
    );
  // clang-format on
  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("FnCallExpr 'f() + 2'", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::Identifier, "f", 0},
    Token{TokenId::LParen, "", 0},
    Token{TokenId::RParen, "", 0},
    Token{TokenId::Plus, "", 0},
    Token{TokenId::IntegerLiteral, "2", 0},
    Token{TokenId::Eof, "", 0}
  };

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::ADD,
    std::make_shared<FnCallExprNode>("f"),
    std::make_shared<IntegerExprNode>("2")
    );
  // clang-format on
  const std::string input{};
  ParserCtxt ctxt(inputTokens, input);
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}
