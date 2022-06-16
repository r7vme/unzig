#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "sema.hpp"
#include "tokenizer.hpp"
#include "code_example.hpp"

TEST_CASE("functions", "[sema]") {
  std::string input = codeExample;
  auto tokens = tokenize(input);
  auto ast = parse(tokens, input);
  SemanticAnalyzer sema;
  ast.sema(&sema);
  REQUIRE(true);
}
