#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "ast.hpp"
#include "codegen.hpp"

using Catch::Matchers::Equals;

TEST_CASE("", "[codegen]") {
  std::string expected = R"(; ModuleID = 'unzig'
source_filename = "unzig"

define void @main() {
entry:
  ret void
}

define void @f() {
entry:
  ret void
}
)";

  auto root = std::make_shared<RootNode>();
  root->declarations.push_back(
      std::make_shared<FnDefNode>("main", UzType{UzTypeId::Void}, nullptr));
  root->declarations.push_back(
      std::make_shared<FnDefNode>("f", UzType{UzTypeId::Void}, nullptr));
  CodeGenerator c;
  REQUIRE(root->codegen(&c));

  std::string output;
  llvm::raw_string_ostream rso(output);
  c.getLLVMModule().print(rso, nullptr);
  REQUIRE_THAT(output, Equals(expected));
}
