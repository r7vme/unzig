#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>

#include "codegen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

void processInput(const std::string &input) {
  auto tokens = tokenize(input);
  auto ast = parse(std::move(tokens));
  if (ast) {
    CodeGenerator generator;
    if (auto *code = ast->codegen(&generator)) {
      std::cout << "=== LLVM IR ===" << std::endl;
      code->print(llvm::errs());
      std::cout << "\n=== LLVM IR ===" << std::endl;
    }
  }
}

int main() {
  std::string inputLine;

  while (true) {
    inputLine = readline("unzig > ");
    processInput(inputLine);
  }
}
