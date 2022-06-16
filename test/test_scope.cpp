#include <catch2/catch_test_macros.hpp>

#include "scope.hpp"

TEST_CASE("test parent scope lookup", "[scope]") {
  auto parent = std::make_shared<ScopeObject>();
  parent->insertSymbol(std::make_shared<SymbolObject>("s1", SymbolType::Var));
  auto scope = std::make_shared<ScopeObject>();
  scope->insertSymbol(std::make_shared<SymbolObject>("s2", SymbolType::Var));
  scope->setParent(parent);

  REQUIRE(scope->lookupSymbol("s1"));
  REQUIRE(scope->lookupSymbol("s2"));
  REQUIRE_FALSE(scope->lookupSymbol("doesNotExist"));
}
