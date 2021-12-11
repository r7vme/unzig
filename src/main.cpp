#include <iostream>

#include "tokenizer.hpp"

int main() {
  std::string s = R"(pub fn main() void {})";

  auto tokens = tokenize(s);

  std::cout << "Input:\n\n" << s << "\n" << std::endl;

  std::cout << "Tokens:\n\n";
  for (auto &t : tokens) {
    std::cout << t << " ";
  }
  std::cout << std::endl;

  std::cout << "success" << std::endl;
  return EXIT_SUCCESS;
}
