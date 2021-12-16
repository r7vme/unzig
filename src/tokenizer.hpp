#pragma once

#include <string>
#include <vector>

enum class TokenId {
  Eof,
  Number,
  Identifier,
  Asterisk,
  Colon,
  Equal,
  LBrace,
  LParen,
  Minus,
  Plus,
  RBrace,
  RParen,
  Semicolon,
  Slash,
  KwFn,
  KwPub,
  KwReturn,
  KwVar,
};

struct Token {
  TokenId id;
  std::string value{};

  friend std::ostream &operator<<(std::ostream &os, const Token &o);
};

bool operator==(const Token& lhs, const Token& rhs);

std::vector<Token> tokenize(const std::string &in);
