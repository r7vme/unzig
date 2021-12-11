#pragma once

#include <string>
#include <vector>

enum class TokenId {
  Eof,
  Float,
  Integer,
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

  friend std::ostream &operator<<(std::ostream &os, const Token &o);
};

std::vector<Token> tokenize(const std::string &in);
