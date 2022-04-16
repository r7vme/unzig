#pragma once
#include "ast.hpp"
#include "tokenizer.hpp"
#include <memory>

class ParserCtxt {
  Tokens tokens;
  size_t cursor{0};

public:
  ParserCtxt(Tokens &&tokens) : tokens(std::move(tokens)){};
  Token getNextToken() { return tokens[cursor++]; }
};

AstNodePtr parse(Tokens &&tokens);
