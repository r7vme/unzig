#include <catch2/catch_test_macros.hpp>

#include "tokenizer.hpp"

TEST_CASE("function + main", "[tokenizer]") {
  std::string s = R"(
fn foo() u64 {
  return 1;
}

pub fn main() void {
  var x: u64 = foo();
  var y: u64 = 123;
  y = 1 + 2;
}
)";
  Tokens expectedTokens = {
      // fn foo
      Token{TokenId::KwFn},
      Token{TokenId::Identifier, "foo"},
      Token{TokenId::LParen},
      Token{TokenId::RParen},
      Token{TokenId::Identifier, "u64"},
      Token{TokenId::LBrace},
      // foo block
      Token{TokenId::KwReturn},
      Token{TokenId::IntegerLiteral, "1"},
      Token{TokenId::Semicolon},
      // end foo block
      Token{TokenId::RBrace},
      // fn main
      Token{TokenId::KwPub},
      Token{TokenId::KwFn},
      Token{TokenId::Identifier, "main"},
      Token{TokenId::LParen},
      Token{TokenId::RParen},
      Token{TokenId::Identifier, "void"},
      Token{TokenId::LBrace},
      // main block
      // line 1
      Token{TokenId::KwVar},
      Token{TokenId::Identifier, "x"},
      Token{TokenId::Colon},
      Token{TokenId::Identifier, "u64"},
      Token{TokenId::Equal},
      Token{TokenId::Identifier, "foo"},
      Token{TokenId::LParen},
      Token{TokenId::RParen},
      Token{TokenId::Semicolon},
      // line 2
      Token{TokenId::KwVar},
      Token{TokenId::Identifier, "y"},
      Token{TokenId::Colon},
      Token{TokenId::Identifier, "u64"},
      Token{TokenId::Equal},
      Token{TokenId::IntegerLiteral, "123"},
      Token{TokenId::Semicolon},
      // line 3
      Token{TokenId::Identifier, "y"},
      Token{TokenId::Equal},
      Token{TokenId::IntegerLiteral, "1"},
      Token{TokenId::Plus},
      Token{TokenId::IntegerLiteral, "2"},
      Token{TokenId::Semicolon},
      // end main block
      Token{TokenId::RBrace},
      Token{TokenId::Eof},
  };

  auto actualTokens = tokenize(s);

  // debug
  // for (auto &token : actualTokens) {
  //   std::cout << token << std::endl;
  // }

  REQUIRE(actualTokens == expectedTokens);
}

TEST_CASE("last char is part of identifier or number", "[tokenizer]") {
  REQUIRE(tokenize("1") ==
          Tokens{Token{TokenId::IntegerLiteral, "1"}, Token{TokenId::Eof}});
  REQUIRE(tokenize("foo") ==
          Tokens{Token{TokenId::Identifier, "foo"}, Token{TokenId::Eof}});
}

TEST_CASE("empty string", "[tokenizer]") {
  REQUIRE(tokenize("") == Tokens{Token{TokenId::Eof}});
}
