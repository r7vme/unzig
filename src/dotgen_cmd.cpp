#include <cstdint>
#include <iostream>

#include "ast.hpp"
#include "ast_node.hpp"
#include "dotgen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

AstNode parseRoot(ParserCtxt &ctxt);

void generateDot(const std::string &input) {
  auto tokens = tokenize(input);
  auto ast = parse(tokens, input);
  DotGenerator g;
  ast.dotgen(&g);
  std::cout << g.getDotOutput() << std::endl;
}

int main(int argc, char **argv) {
  std::istreambuf_iterator<char> begin(std::cin), end;
  std::string input(begin, end);
  generateDot(input);
}
