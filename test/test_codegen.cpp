#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "ast.hpp"
#include "codegen.hpp"

using Catch::Matchers::Equals;

TEST_CASE("", "[codegen]") {
  std::string expected = R"(define void @main() {
entry:
  ret void
}
)";

  AstNodePtr fnDef =
      std::make_shared<FnDefNode>("main", UzType{UzTypeId::Void}, nullptr);
  CodeGenerator c;
  auto *code = fnDef->codegen(&c);
  REQUIRE(code);

  std::string output;
  llvm::raw_string_ostream rso(output);
  code->print(rso);
  REQUIRE_THAT(output, Equals(expected));
}
