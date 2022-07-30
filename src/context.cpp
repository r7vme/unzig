#include "context.hpp"
#include "types.hpp"

CompilerContext createCompilerContext(const Source source) {
  auto cc = std::make_shared<CompilerContextObject>();
  cc->source = source;
  addBuiltInTypes(cc->typeTable);
  return cc;
}
