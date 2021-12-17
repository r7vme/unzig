#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "tokenizer.hpp"

// clang-format off
#define ASTERISK '*'
#define COLON ':'
#define EQUAL '='
#define LBRACE '{'
#define LPAREN '('
#define MINUS '-'
#define PLUS '+'
#define RBRACE '}'
#define RPAREN ')'
#define SEMICOLON ';'
#define SLASH '/'

#define DOT '.'

#define KW_FN "fn"
#define KW_PUB "pub"
#define KW_RETURN "return"
#define KW_VAR "var"

#define SINGLE_CHAR_TOKENS \
  ASTERISK: \
  case COLON: \
  case EQUAL: \
  case LBRACE: \
  case LPAREN: \
  case MINUS: \
  case PLUS: \
  case RBRACE: \
  case RPAREN: \
  case SEMICOLON: \
  case SLASH

#define WHITESPACE \
       ' ': \
  case '\r': \
  case '\n'

#define DIGIT \
         '0': \
    case '1': \
    case '2': \
    case '3': \
    case '4': \
    case '5': \
    case '6': \
    case '7': \
    case '8': \
    case '9'

#define ALPHA \
       'a': \
  case 'b': \
  case 'c': \
  case 'd': \
  case 'e': \
  case 'f': \
  case 'g': \
  case 'h': \
  case 'i': \
  case 'j': \
  case 'k': \
  case 'l': \
  case 'm': \
  case 'n': \
  case 'o': \
  case 'p': \
  case 'q': \
  case 'r': \
  case 's': \
  case 't': \
  case 'u': \
  case 'v': \
  case 'w': \
  case 'x': \
  case 'y': \
  case 'z': \
  case 'A': \
  case 'B': \
  case 'C': \
  case 'D': \
  case 'E': \
  case 'F': \
  case 'G': \
  case 'H': \
  case 'I': \
  case 'J': \
  case 'K': \
  case 'L': \
  case 'M': \
  case 'N': \
  case 'O': \
  case 'P': \
  case 'Q': \
  case 'R': \
  case 'S': \
  case 'T': \
  case 'U': \
  case 'V': \
  case 'W': \
  case 'X': \
  case 'Y': \
  case 'Z'

#define ALPHA_DIGIT \
  ALPHA: \
  case DIGIT

#define KNOWN_CHARS \
  SINGLE_CHAR_TOKENS: \
  case WHITESPACE: \
  case ALPHA: \
  case DOT: \
  case DIGIT
// clang-format on

enum class TokenizeState {
  Begin,
  Identifier,
  Number,
};

static std::map<std::string, TokenId> keywordsNameMap{
    {KW_FN, TokenId::KwFn},
    {KW_PUB, TokenId::KwPub},
    {KW_RETURN, TokenId::KwReturn},
    {KW_VAR, TokenId::KwVar},
};

// clang-format off
static std::map<char, TokenId> specialCharsNameMap{
    {ASTERISK, TokenId::Asterisk},
    {COLON, TokenId::Colon},
    {EQUAL, TokenId::Equal},
    {LBRACE, TokenId::LBrace},
    {LPAREN, TokenId::LParen},
    {MINUS, TokenId::Minus},
    {PLUS, TokenId::Plus},
    {RBRACE, TokenId::RBrace},
    {RPAREN, TokenId::RParen},
    {SEMICOLON, TokenId::Semicolon},
    {SLASH, TokenId::Slash},
};
// clang-format on

static std::string getTokenIdName(TokenId id) {
  // clang-format off
  switch (id) {
  case TokenId::Eof: return "Eof";
  case TokenId::Float: return "Float";
  case TokenId::Integer: return "Integer";
  case TokenId::Identifier: return "Identifier";
  case TokenId::Asterisk: return "Asterisk";
  case TokenId::Colon: return "Colon";
  case TokenId::Equal: return "Equal";
  case TokenId::LBrace: return "LBrace";
  case TokenId::LParen: return "LParen";
  case TokenId::Minus: return "Minus";
  case TokenId::Plus: return "Plus";
  case TokenId::RBrace: return "RBrace";
  case TokenId::RParen: return "RParen";
  case TokenId::Semicolon: return "Semicolon";
  case TokenId::Slash: return "Slash";
  case TokenId::KwFn: return "KwFn";
  case TokenId::KwPub: return "KwPub";
  case TokenId::KwReturn: return "KwReturn";
  case TokenId::KwVar: return "KwVar";
  default: assert(false);
  }
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Token &o) {
  return os << "Token ID: " << getTokenIdName(o.id)
            << ", Token Value: " << o.value;
}

bool operator==(const Token &lhs, const Token &rhs) {
  return (lhs.id == rhs.id) && (lhs.value == rhs.value);
}

void fatal_error(const std::string &msg) {
  std::cerr << "fatal: " << msg << std::endl;
  std::exit(1);
}

bool isKeyword(const std::string &s) {
  auto search = keywordsNameMap.find(s);
  return (search != keywordsNameMap.end());
}

TokenId getKeywordTokenId(const std::string &s) {
  auto search = keywordsNameMap.find(s);
  if (search == keywordsNameMap.end()) {
    fatal_error(std::string("unknown token ") + s);
  }
  return search->second;
}

TokenId getSpecialCharTokenId(char c) {
  auto search = specialCharsNameMap.find(c);
  if (search == specialCharsNameMap.end()) {
    fatal_error(std::string("unknown special char ") + c);
  }
  return search->second;
}

Token getKeywordOrIdentifierTokenFromString(const std::string &s) {
  if (isKeyword(s)) {
    return Token{getKeywordTokenId(s)};
  }
  return Token{TokenId::Identifier, s};
}

bool isKnownChar(char c) {
  switch (c) {
  case KNOWN_CHARS:
    return true;
  default:
    return false;
  }
}

std::vector<Token> tokenize(const std::string &in) {
  std::vector<Token> tokens;
  std::string identifierStr{};
  TokenizeState state{TokenizeState::Begin};

  for (auto it = std::begin(in); it != std::end(in); ++it) {
    char c = *it;

    if (!isKnownChar(c)) {
      fatal_error(std::string("unknown char ") + c);
    }

    switch (state) {
    case TokenizeState::Begin:
      identifierStr.clear();
      switch (c) {
      case WHITESPACE:
        break;
      case SINGLE_CHAR_TOKENS:
        tokens.push_back(Token{getSpecialCharTokenId(c)});
        break;
      case ALPHA:
        identifierStr += c;
        state = TokenizeState::Identifier;
        break;
      case DIGIT:
        identifierStr += c;
        state = TokenizeState::Number;
        break;
      default:;
      }
      break;
    case TokenizeState::Identifier:
      switch (c) {
      case ALPHA_DIGIT:
        identifierStr += c;
        break;
      default:
        tokens.push_back(getKeywordOrIdentifierTokenFromString(identifierStr));
        state = TokenizeState::Begin;
        std::advance(it, -1);
        break;
      }
      break;
    case TokenizeState::Number:
      switch (c) {
      case DOT:
      case DIGIT:
        identifierStr += c;
        break;
      default:
        bool isFloat = identifierStr.find(DOT) != std::string::npos;
        if (isFloat) {
          tokens.push_back(Token{TokenId::Float, identifierStr});
        } else {
          tokens.push_back(Token{TokenId::Integer, identifierStr});
        }
        state = TokenizeState::Begin;
        std::advance(it, -1);
      }
      break;
    default:;
    }
  }

  return tokens;
}
