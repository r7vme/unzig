#include <iostream>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "ast.hpp"
#include "ast_equality_comparator.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "types.hpp"

AstNodePtr parseExpr(ParserCtxt &ctxt);
AstNodePtr parseVarDecl(ParserCtxt &ctxt);
AstNodePtr parseFnDef(ParserCtxt &ctxt);

TEST_CASE("BinOpRhsExpr inverted operation priority - 1 + 2 * 3", "[parser]") {
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
  ParserCtxt ctxt(std::move(inputTokens));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("BinOpRhsExpr - 1 * 2 + 3", "[parser]") {
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
  ParserCtxt ctxt(std::move(inputTokens));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("GroupedExpr - 3 * (2 + 1)", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::IntegerLiteral, "3"},
    Token{TokenId::Asterisk},
    Token{TokenId::LParen},
    Token{TokenId::IntegerLiteral, "2"},
    Token{TokenId::Plus},
    Token{TokenId::IntegerLiteral, "1"},
    Token{TokenId::RParen},
    Token{TokenId::Eof}
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
  ParserCtxt ctxt(std::move(inputTokens));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("floating point numbers expr", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::FloatLiteral, "1.0"},
    Token{TokenId::Asterisk},
    Token{TokenId::FloatLiteral, "2.0"},
    Token{TokenId::Eof}
  };

  AstNodePtr expectedAST = std::make_shared<BinExprNode>(
    BinOpType::MUL,
    std::make_shared<FloatExprNode>("1.0"),
    std::make_shared<FloatExprNode>("2.0")
  );
  // clang-format on
  ParserCtxt ctxt(std::move(inputTokens));
  auto AST = parseExpr(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("VarDecl 'var y: i32 = 123;'", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::KwVar},
    Token{TokenId::Identifier, "y"},
    Token{TokenId::Colon},
    Token{TokenId::Identifier, "i32"},
    Token{TokenId::Equal},
    Token{TokenId::IntegerLiteral, "123"},
    Token{TokenId::Semicolon},
    Token{TokenId::Eof}
  };

  AstNodePtr expectedAST = std::make_shared<VarDeclNode>(
    "y",
    UzType{UzTypeId::Int32},
    std::make_shared<IntegerExprNode>("123")
  );
  // clang-format on

  ParserCtxt ctxt(std::move(inputTokens));
  auto AST = parseVarDecl(ctxt);
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}

TEST_CASE("FnDef 'fn main() void {};'", "[parser]") {
  // clang-format off
  Tokens inputTokens = {
    Token{TokenId::KwFn},
    Token{TokenId::Identifier, "main"},
    Token{TokenId::LParen},
    Token{TokenId::RParen},
    Token{TokenId::Identifier, "void"},
    Token{TokenId::LBrace},
    Token{TokenId::RBrace},
    Token{TokenId::Eof}
  };
  // clang-format on

  std::vector<AstNodePtr> declarations;
  declarations.push_back(
      std::make_shared<FnDefNode>("main", UzType{UzTypeId::Void}, nullptr));
  auto expectedAST = std::make_shared<RootNode>(declarations);

  auto AST = parse(std::move(inputTokens));
  REQUIRE(AST);

  AstEqualityComparator c;
  REQUIRE(AST->isEqual(&c, *expectedAST));
}
