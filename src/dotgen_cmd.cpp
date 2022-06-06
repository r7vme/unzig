#include <iostream>

#include "dotgen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

AstNodePtr parseExpr(ParserCtxt &ctxt);
AstNodePtr parseRoot(ParserCtxt &ctxt);

void generateDot(const std::string &input) {
  auto tokens = tokenize(input);

  ParserCtxt ctxt(tokens, input);
  auto ast = parseRoot(ctxt);
  if (ast) {
    std::string output;
    DotGenerator g;
    ast->dotgen(&g, output);

    std::string header = "digraph \"AST\" {\n";
    std::string footer = "}";
    std::cout << header << output << footer << std::endl;
  } else {
    std::cerr << "Unable to parse input" << std::endl;
    std::exit(1);
  }
}

int main(int argc, char **argv) {
  std::istreambuf_iterator<char> begin(std::cin), end;
  std::string input(begin, end);
  generateDot(input);
}
