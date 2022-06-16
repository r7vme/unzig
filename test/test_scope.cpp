#include <catch2/catch_test_macros.hpp>

#include "scope.hpp"
#include "types.hpp"

TEST_CASE("test parent scope lookup", "[scope]") {
  UzType type({UzTypeId::Int32});
  auto parent = std::make_shared<ScopeObject>();
  parent->insertSymbol(std::make_shared<SymbolObject>(SymbolType::Var, "s1", type, false));
  auto scope = std::make_shared<ScopeObject>();
  scope->insertSymbol(std::make_shared<SymbolObject>(SymbolType::Var, "s2", type, false));
  scope->setParent(parent);

  REQUIRE(scope->lookupSymbol("s1"));
  REQUIRE(scope->lookupSymbol("s2"));
  REQUIRE_FALSE(scope->lookupSymbol("doesNotExist"));
}
