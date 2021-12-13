#include <vector>
#include <catch2/catch_test_macros.hpp>

#include "tokenizer.hpp"

TEST_CASE("tokenizer", "[tokenizer]") {
  std::string s = R"(
fn foo() u64 {
  return 1;
}

pub fn main() void {
  var x: u64 = foo();
  var y: u64 = 123;
  y = 1;
}
)";
  std::vector<Token> expected_tokens = {
    // fn foo
    Token{TokenId::KwFn},
    Token{TokenId::Identifier},
    Token{TokenId::LParen},
    Token{TokenId::RParen},
    Token{TokenId::Identifier},
    Token{TokenId::LBrace},
      // foo block
      Token{TokenId::KwReturn},
      Token{TokenId::Number},
      Token{TokenId::Semicolon},
    Token{TokenId::RBrace},
    // fn main
    Token{TokenId::KwPub},
    Token{TokenId::KwFn},
    Token{TokenId::Identifier},
    Token{TokenId::LParen},
    Token{TokenId::RParen},
    Token{TokenId::Identifier},
    Token{TokenId::LBrace},
      // main block
      // line 1
      Token{TokenId::KwVar},
      Token{TokenId::Identifier},
      Token{TokenId::Colon},
      Token{TokenId::Identifier},
      Token{TokenId::Equal},
      Token{TokenId::Identifier},
      Token{TokenId::LParen},
      Token{TokenId::RParen},
      Token{TokenId::Semicolon},
      // line 2
      Token{TokenId::KwVar},
      Token{TokenId::Identifier},
      Token{TokenId::Colon},
      Token{TokenId::Identifier},
      Token{TokenId::Equal},
      Token{TokenId::Number},
      Token{TokenId::Semicolon},
      // line 3
      Token{TokenId::Identifier},
      Token{TokenId::Equal},
      Token{TokenId::Number},
      Token{TokenId::Semicolon},
    Token{TokenId::RBrace},
  };

  auto actual_tokens = tokenize(s);

  REQUIRE(actual_tokens == expected_tokens);
}
