#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <filesystem>

#include "ast.hpp"
#include "codegen.hpp"
#include "dotgen.hpp"
#include "parser.hpp"
#include "sema.hpp"
#include "tokenizer.hpp"

namespace fs = std::filesystem;

TEST_CASE("generates valid .dot graph", "[dotgen]") {
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

  INFO("checking if dot is intalled");
  REQUIRE(std::system("hash dot") == 0);

  auto tokens = tokenize(input);
  auto ast = parse(tokens, input);
  DotGenerator g;
  ast.dotgen(&g);

  std::string dotTmpFile = fs::temp_directory_path() / "test_dotgen.dot";
  std::ofstream ofs(dotTmpFile);
  ofs << g.getDotOutput();
  ofs.close();

  auto dotCmd = std::string("dot -Tpng ") + dotTmpFile + " > /dev/null";
  INFO("checking if correct .dot file was generated");
  CHECK(std::system(dotCmd.c_str()) == 0);
  fs::remove(dotTmpFile);
}
