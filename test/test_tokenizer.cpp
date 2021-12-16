#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>

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
  std::vector<Token> expected_tokens = {
      // fn foo
      Token{TokenId::KwFn},
      Token{TokenId::Identifier, "foo"},
      Token{TokenId::LParen},
      Token{TokenId::RParen},
      Token{TokenId::Identifier, "u64"},
      Token{TokenId::LBrace},
      // foo block
      Token{TokenId::KwReturn},
      Token{TokenId::Number, "1"},
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
      Token{TokenId::Number, "123"},
      Token{TokenId::Semicolon},
      // line 3
      Token{TokenId::Identifier, "y"},
      Token{TokenId::Equal},
      Token{TokenId::Number, "1"},
      Token{TokenId::Plus},
      Token{TokenId::Number, "2"},
      Token{TokenId::Semicolon},
      // end main block
      Token{TokenId::RBrace},
  };

  auto actual_tokens = tokenize(s);

  // debug
  for (auto &token : actual_tokens) {
    std::cout << token << std::endl;
  }

  REQUIRE(actual_tokens == expected_tokens);
}
