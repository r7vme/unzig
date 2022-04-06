#include "parser.hpp"
#include "tokenizer.hpp"

#include <iostream>

void processInput(const std::string &input) {
  auto tokens = tokenize(input);
  auto ast = parse(tokens);
  // print LLVM IR
  if (auto *code = ast->codegen()) {
    std::cout << "==========" << std::endl;
    code->print(llvm::errs());
    std::cout << "\n==========" << std::endl;
  }
}

int main() {
  std::string inputLine;

  while (true) {
    std::cout << "unzig > ";
    std::getline(std::cin, inputLine);
    processInput(inputLine);
  }
}
