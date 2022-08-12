#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <llvm-14/llvm/IR/CallingConv.h>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "tokenizer.hpp"

// clang-format off
// single char tokens
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
#define RARROW '>'
#define LARROW '<'
#define EXCLAMATIONMARK '!'

// two char tokens
#define EQUALEQUAL "=="
#define RARROWEQUAL ">="
#define LARROWEQUAL "<="
#define EXCLAMATIONMARKEQUAL "!="

#define DOT '.'

#define KW_FN "fn"
#define KW_PUB "pub"
#define KW_RETURN "return"
#define KW_VAR "var"
#define KW_IF "if"
#define KW_ELSE "else"
#define KW_TRUE "true"
#define KW_FALSE "false"
#define KW_AND "and"
#define KW_OR "or"

#define SPECIAL_CHARS \
  ASTERISK: \
  case COLON: \
  case EQUAL: \
  case LBRACE: \
  case LPAREN: \
  case MINUS: \
  case PLUS: \
  case RBRACE: \
  case RPAREN: \
  case EXCLAMATIONMARK: \
  case RARROW: \
  case LARROW: \
  case SLASH: \
  case SEMICOLON

#define NEWLINE \
       '\r': \
  case '\n'

#define SKIP \
       ' ': \
  case NEWLINE

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
  SPECIAL_CHARS: \
  case SKIP: \
  case ALPHA: \
  case DOT: \
  case DIGIT
// clang-format on

enum class TokenizeState {
  Begin,
  Identifier,
  Number,
  Comment,
};

// clang-format off
static std::map<std::string, TokenId> keywordTokenIds{
    {KW_FN, TokenId::KwFn},
    {KW_PUB, TokenId::KwPub},
    {KW_RETURN, TokenId::KwReturn},
    {KW_VAR, TokenId::KwVar},
    {KW_IF, TokenId::KwIf},
    {KW_ELSE, TokenId::KwElse},
    {KW_TRUE, TokenId::KwTrue},
    {KW_FALSE, TokenId::KwFalse},
    {KW_AND, TokenId::KwAnd},
    {KW_OR, TokenId::KwOr},
};

static std::map<std::string, TokenId> twoCharsTokenIds{
    {EQUALEQUAL, TokenId::EqualEqual},
    {RARROWEQUAL, TokenId::RArrowEqual},
    {LARROWEQUAL, TokenId::LArrowEqual},
    {EXCLAMATIONMARKEQUAL, TokenId::ExclamationMarkEqual},
};

static std::map<char, TokenId> singleCharTokenIds{
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
    {RARROW, TokenId::RArrow},
    {LARROW, TokenId::LArrow},
    {EXCLAMATIONMARK, TokenId::ExclamationMark},
};
// clang-format on

static std::string getTokenIdName(TokenId id) {
  // clang-format off
  switch (id) {
  case TokenId::Eof: return "Eof";
  case TokenId::FloatLiteral: return "FloatLiteral";
  case TokenId::IntegerLiteral: return "IntegerLiteral";
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
  case TokenId::LArrow: return "LArrow";
  case TokenId::RArrow: return "RArrow";
  case TokenId::RArrowEqual: return "RArrowEqual";
  case TokenId::LArrowEqual: return "LArrowEqual";
  case TokenId::EqualEqual: return "EqualEqual";
  case TokenId::ExclamationMark: return "ExclamationMark";
  case TokenId::ExclamationMarkEqual: return "ExclamationMarkEqual";
  case TokenId::KwFn: return "KwFn";
  case TokenId::KwPub: return "KwPub";
  case TokenId::KwReturn: return "KwReturn";
  case TokenId::KwVar: return "KwVar";
  case TokenId::KwIf: return "KwIf";
  case TokenId::KwElse: return "KwElse";
  case TokenId::KwTrue: return "KwTrue";
  case TokenId::KwFalse: return "KwFalse";
  case TokenId::KwAnd: return "KwAnd";
  case TokenId::KwOr: return "KwOr";
  default: assert(false);
  }
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Token &o) {
  return os << "Token ID: " << getTokenIdName(o.id) << ", Token Value: " << o.value;
}

bool operator==(const Token &lhs, const Token &rhs) {
  return (lhs.id == rhs.id) && (lhs.value == rhs.value);
}

void fatalLexerError(const std::string &msg) {
  std::cerr << "Lexer error: " << msg << std::endl;
  std::exit(EXIT_FAILURE);
}

bool isKeyword(const std::string &s) {
  auto search = keywordTokenIds.find(s);
  return (search != keywordTokenIds.end());
}

TokenId getKeywordTokenId(const std::string &s) {
  auto search = keywordTokenIds.find(s);
  if (search == keywordTokenIds.end()) {
    fatalLexerError(std::string("unknown token ") + s);
  }
  return search->second;
}

std::optional<TokenId> getTwoCharsTokenId(const std::string &s, const size_t position) {
  if ((position + 2) > s.size()) {
    return std::nullopt;
  }

  auto search = twoCharsTokenIds.find(s.substr(position, 2));
  if (search == twoCharsTokenIds.end()) {
    return std::nullopt;
  }
  return search->second;
}

TokenId getSingleCharTokenId(const char c) {
  auto search = singleCharTokenIds.find(c);
  if (search == singleCharTokenIds.end()) {
    fatalLexerError(std::string("unknown special char ") + c);
  }
  return search->second;
}

Token getKeywordOrIdentifierTokenFromString(const std::string &s, const size_t tokenPosition) {
  if (isKeyword(s)) {
    return Token{getKeywordTokenId(s), "", tokenPosition};
  }
  return Token{TokenId::Identifier, s, tokenPosition};
}

bool isKnownChar(char c) {
  switch (c) {
  case KNOWN_CHARS:
    return true;
  default:
    return false;
  }
}

std::vector<Token> tokenize(const Source source) {
  auto &in = source->getRawSource();

  std::vector<Token> tokens;
  std::string identifierStr{};
  TokenizeState state{TokenizeState::Begin};

  for (size_t i = 0; i <= in.size(); ++i) {
    char c;

    if (i == in.size()) {
      c = ' '; // extra whitespace lets wrap a token that can be in
               // progress
    } else {
      c = in[i];
    }

    if (!isKnownChar(c)) {
      fatalLexerError(std::string("unknown char ") + c);
    }

    switch (state) {
    case TokenizeState::Begin:
      identifierStr.clear();
      switch (c) {
      case SKIP:
        break;
      case SPECIAL_CHARS:
        if (in[i + 1] == SLASH) {
          state = TokenizeState::Comment;
          ++i;
        } else if (auto maybeTokenId = getTwoCharsTokenId(in, i)) {
          tokens.push_back(Token{maybeTokenId.value(), "", i});
          ++i;
        } else {
          tokens.push_back(Token{getSingleCharTokenId(c), "", i});
        }
        break;
      case ALPHA:
        identifierStr += c;
        state = TokenizeState::Identifier;
        break;
      case DIGIT:
        identifierStr += c;
        state = TokenizeState::Number;
        break;
      default:
        break;
      }
      break;
    case TokenizeState::Identifier:
      switch (c) {
      case ALPHA_DIGIT:
        identifierStr += c;
        break;
      default:
        auto tokenPosition = (i - identifierStr.size());
        tokens.push_back(getKeywordOrIdentifierTokenFromString(identifierStr, tokenPosition));
        state = TokenizeState::Begin;
        i--;
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
        auto tokenPosition = (i - identifierStr.size());
        bool isFloat = identifierStr.find(DOT) != std::string::npos;
        if (isFloat) {
          tokens.push_back(Token{TokenId::FloatLiteral, identifierStr, tokenPosition});
        } else {
          tokens.push_back(Token{TokenId::IntegerLiteral, identifierStr, tokenPosition});
        }
        state = TokenizeState::Begin;
        i--;
        break;
      }
      break;
    case TokenizeState::Comment:
      switch (c) {
      case NEWLINE:
        state = TokenizeState::Begin;
        break;
      default:
        break;
      }
    default:;
    }
  }

  tokens.push_back(Token{TokenId::Eof, "", in.size()});

  return tokens;
}
