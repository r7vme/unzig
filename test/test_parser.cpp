#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "ast_equality_comparator.hpp"
#include "parser.hpp"

TEST_CASE("1 + 2 * 3", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1"},
    Token{TokenId::Plus},
    Token{TokenId::IntegerLiteral, "2"},
    Token{TokenId::Asterisk},
    Token{TokenId::IntegerLiteral, "3"},
    Token{TokenId::Eof}
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
  auto AST = parse(std::move(inputTokens));
  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("1 * 2 + 3", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "1"},
    Token{TokenId::Asterisk},
    Token{TokenId::IntegerLiteral, "2"},
    Token{TokenId::Plus},
    Token{TokenId::IntegerLiteral, "3"},
    Token{TokenId::Eof}
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
  auto AST = parse(std::move(inputTokens));
  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}
