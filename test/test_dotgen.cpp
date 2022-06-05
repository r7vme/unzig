#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <vector>
#include <iostream>

#include "ast.hpp"
#include "dotgen.hpp"

using Catch::Matchers::Equals;

TEST_CASE("dotgen", "[dotgen]") {
  AstNodePtr AST = std::make_shared<BinExprNode>(
    BinOpType::ADD,
    std::make_shared<IntegerExprNode>("1"),
    std::make_shared<BinExprNode>(
      BinOpType::MUL,
      std::make_shared<IntegerExprNode>("2"),
      std::make_shared<FloatExprNode>("3.1")
    )
  );

  std::string dotData;
  DotGenerator g;
  AST->dotgen(&g, dotData);

  std::cout << dotData << std::endl;
}
