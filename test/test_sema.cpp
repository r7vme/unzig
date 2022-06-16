#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <vector>

#include "ast.hpp"
#include "codegen.hpp"
#include "parser.hpp"
#include "sema.hpp"
#include "tokenizer.hpp"

TEST_CASE("functions", "[sema]") {
  std::string input = R"(
var gvar: i32 = 123;

fn foo() i32 {
    return 1.0;
}

fn main() void {
    var x: i32 = foo();
    var y: i32 = 123 + (1 * x);
    y = 1;
}
)";

  auto tokens = tokenize(input);
  auto ast = parse(tokens, input);
  SemanticAnalyzer sema;
  ast.sema(&sema);
  REQUIRE(true);
}
