#include "context.hpp"

CompilerContext createCompilerContext(const Source source) {
  auto cc = std::make_shared<CompilerContextObject>();
  cc->source = source;
  return cc;
}
