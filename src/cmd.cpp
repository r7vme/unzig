#include "codegen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

#include <iostream>

void processInput(const std::string &input) {
  auto tokens = tokenize(input);
  auto ast = parse(std::move(tokens));

  CodeGenerator generator;
  if (auto *code = ast->codegen(&generator)) {
    std::cout << "=== LLVM IR ===" << std::endl;
    code->print(llvm::errs());
    std::cout << "\n=== LLVM IR ===" << std::endl;
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
