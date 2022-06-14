#include <iostream>

#include "ast.hpp"
#include "dotgen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

MayBeAstNode parseExpr(ParserCtxt &ctxt);
MayBeAstNode parseRoot(ParserCtxt &ctxt);

void generateDot(const std::string &input) {
  auto tokens = tokenize(input);
  ParserCtxt ctxt(tokens, input);
  auto ast = parseRoot(ctxt).value();
  DotGenerator g;
  ast.dotgen(&g);
  std::cout << g.getDotOutput() << std::endl;
}

int main(int argc, char **argv) {
  std::istreambuf_iterator<char> begin(std::cin), end;
  std::string input(begin, end);
  generateDot(input);
}
