#include "tokenizer.hpp"
#include <memory>

// Parse 1 + 1

class AstNode {
public:
  virtual ~AstNode() {}
};

// Emits constant int
class IntegerNode : public AstNode {
  const std::string value;

public:
  IntegerNode(double val) : val(val) {}
};

//
struct ParseContext {
  std::shared_ptr<Tokens> tokens;
  Tokens::iterator tokensCursor;
};

static std::unique_ptr<AstNode> AstParseRoot(ParseContext &cxt) {
  std::unique_ptr<AstNode> result{nullptr};

  auto tokenId = (*cxt.tokensCursor).id;
  auto tokenValue = (*cxt.tokensCursor).value;

  switch (tokenId) {
  case TokenId::Float:
    result = std::make_unique<FloatExprAST>(std::stod(tokenValue));
  case TokenId::Integer:
    result = std::make_unique<FloatExprAST>(std::stod(tokenValue));
  default:
    break;
  }

  return result;
}

///// top ::= definition | external | expression | ';'
//
// static void MainLoop() {
//  while (1) {
//    fprintf(stderr, "ready> ");
//    switch (CurTok) {
//    case tok_eof:
//      return;
//    case ';': // ignore top-level semicolons.
//      getNextToken();
//      break;
//    case tok_def:
//      HandleDefinition();
//      break;
//    case tok_extern:
//      HandleExtern();
//      break;
//    default:
//      HandleTopLevelExpression();
//      break;
//    }
//  }
//}

int main() {
  auto tokensPtr = std::make_shared<Tokens>(Tokens{
      Token{TokenId::Integer, "1"},
      Token{TokenId::Plus},
      Token{TokenId::Integer, "2"},
  });
  auto firstToken = tokensPtr->begin();

  ParseContext context;
  context.tokens = tokensPtr;
  context.tokensCursor = firstToken;

  return 0;
  //
}
