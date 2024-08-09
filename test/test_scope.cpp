#include <catch2/catch_test_macros.hpp>

#include "scope.hpp"
#include "symbol.hpp"
#include "types.hpp"

TEST_CASE("test parent scope lookup", "[scope]") {
  UzType type = std::make_shared<UzTypeObject>(UzTypeObject{
      .id = UzTypeId::Int,
      .name = "i32",
      .type = IntParams{32, false},
      });
  auto parent = createChildScope(nullptr);
  auto scope = createChildScope(parent);

  parent->insertSymbol(createSymbol(SymbolType::Var, "s1", type, false, 0));
  scope->insertSymbol(createSymbol(SymbolType::Var, "s2", type, false, 0));

  REQUIRE(scope->lookupSymbol("s1"));
  REQUIRE(scope->lookupSymbol("s2"));
  REQUIRE_FALSE(scope->lookupSymbol("doesNotExist"));
}
