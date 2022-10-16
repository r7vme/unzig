#pragma once

#include "source.hpp"
#include <cstdint>
#include <string>
#include <vector>

enum class TokenId {
  Eof,
  FloatLiteral,
  IntegerLiteral,
  Identifier,
  Asterisk,
  Colon,
  Comma,
  Equal,
  LBrace,
  LParen,
  Minus,
  Plus,
  RBrace,
  RParen,
  Semicolon,
  Slash,
  EqualEqual,
  ExclamationMark,
  ExclamationMarkEqual,
  LArrow,
  RArrow,
  LArrowEqual,
  RArrowEqual,
  KwFn,
  KwPub,
  KwReturn,
  KwVar,
  KwIf,
  KwElse,
  KwFalse,
  KwTrue,
  KwAnd,
  KwOr
};

struct Token {
  Token(const TokenId id, const std::string &value, const size_t position)
      : id(id), value(value), position(position) {}

  TokenId id;
  std::string value;
  size_t position;

  friend std::ostream &operator<<(std::ostream &os, const Token &o);
};

using Tokens = std::vector<Token>;

bool operator==(const Token &lhs, const Token &rhs);

Tokens tokenize(const Source in);
