#pragma once
#include <memory>

#include "ast.hpp"
#include "tokenizer.hpp"

class ParserCtxt {
  const Tokens &tokens;
  const std::string &source;
  size_t cursor{0};

public:
  ParserCtxt(const Tokens &tokens, const std::string &source)
      : tokens(tokens), source(source){};

  const Token &getToken() const {
    assert(tokens.size() > cursor);
    return tokens[cursor];
  }

  void skipToken() { cursor++; }

  const Token &getTokenAndAdvance() {
    const Token &t = getToken();
    skipToken();
    return t;
  }

  size_t getCursor() const { return cursor; }
  void resetCursor(const size_t newCursor) {
    assert(tokens.size() > newCursor);
    cursor = newCursor;
  }
};

AstNodePtr parse(const Tokens &tokens, const std::string &source);
