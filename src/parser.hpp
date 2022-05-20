#pragma once
#include <memory>

#include "ast.hpp"
#include "tokenizer.hpp"

class ParserCtxt {
  Tokens tokens;
  size_t cursor{0};

public:
  ParserCtxt(Tokens &&tokens) : tokens(std::move(tokens)){};

  Token &getToken() { return tokens[cursor]; }

  void skipToken() {
    assert(tokens.size() > cursor);
    cursor++;
  }

  Token &getTokenAndAdvance() {
    Token &t = getToken();
    skipToken();
    return t;
  }

  size_t getCursor() const { return cursor; }
  void resetCursor(const size_t newCursor) {
    assert(tokens.size() > newCursor);
    cursor = newCursor;
  }
};

AstNodePtr parse(Tokens &&tokens);
