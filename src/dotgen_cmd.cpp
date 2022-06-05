#include <filesystem>
#include <iostream>
#include <fstream>
#include <readline/history.h>
#include <readline/readline.h>

#include "codegen.hpp"
#include "dotgen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

namespace fs = std::filesystem;

AstNodePtr parseExpr(ParserCtxt &ctxt);

void generateDot(const std::string &input) {
  auto tokens = tokenize(input);

  ParserCtxt ctxt(std::move(tokens));
  auto ast = parseExpr(ctxt);
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


std::string readFileIntoString(const std::string &path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file - '" << path << "'" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return std::string((std::istreambuf_iterator<char>(input_file)),
                     std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: unzig_dotgen <file>.uz" << std::endl;
    std::exit(1);
  }
  fs::path inputFilePath = argv[1];
  if (!fs::exists(inputFilePath)) {
    std::cerr << "File " << inputFilePath << " does not exist" << std::endl;
    std::exit(1);
  }
  std::string inputCode = readFileIntoString(inputFilePath);
  generateDot(inputCode);
}
