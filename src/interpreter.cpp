#include "tokenizer.hpp"

#include <iostream>

void processInput(const std::string &input) {
  // tokenizer
  auto tokens = tokenize(input);
  for (auto &t : tokens) {
    std::cout << t << std::endl;
  }

  // parser
}

int main() {
  std::string inputLine;

  while (true) {
    std::cout << "unzig > ";
    std::getline(std::cin, inputLine);
    processInput(inputLine);
  }
}
