#pragma once
#include <memory>

#include "ast.hpp"
#include "tokenizer.hpp"

class ParserCtxt {
  Tokens tokens;
  size_t cursor{0};

public:
  ParserCtxt(Tokens &&tokens) : tokens(std::move(tokens)){};
  Token& getTokenAndAdvance() { return tokens[cursor++]; }
  Token& getToken() { return tokens[cursor]; }
};

AstNodePtr parse(Tokens &&tokens);
