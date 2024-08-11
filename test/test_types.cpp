#include "catch2/catch_test_macros.hpp"

#include "types.hpp"

TEST_CASE("find type", "[types]") {
  TypeTable *tt = &TypeTable::getInstance();
  REQUIRE(tt->findType("i8")->id == UzTypeId::I8);
  REQUIRE(tt->findType("non-existent-type") == nullptr);
}

TEST_CASE("add type", "[types]") {
  TypeTable *tt = &TypeTable::getInstance();
  REQUIRE_NOTHROW(tt->addType("new", UzType{.id = UzTypeId::F32}));
  REQUIRE_THROWS(tt->addType("new", UzType{.id = UzTypeId::F32}));
}
