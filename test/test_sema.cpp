#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <vector>

#include "ast.hpp"
#include "codegen.hpp"
#include "sema.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"

using Catch::Matchers::Equals;

TEST_CASE("functions", "[sema]") {
  std::string input = R"(
var gvar: i32 = 123;
)";

  auto tokens = tokenize(input);
  auto ast = parse(tokens, input);
  SemanticAnalyzer sema;
  ast.sema(&sema);
  REQUIRE(true);
}
