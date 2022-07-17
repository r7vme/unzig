#include "context.hpp"
#include "types.hpp"

void addBuiltInTypes(TypeTable &t) {
  UzType i32{
      .id = UzTypeId::Int,
      .name = "i32",
      .type = UzTypeInt{32},
  };
  t.insert({i32.name, i32});

  UzType f32{
      .id = UzTypeId::Float,
      .name = "f32",
      .type = UzTypeInt{32},
  };
  t.insert({f32.name, f32});
}

CompilerContext createCompilerContext(const Source source) {
  auto cc = std::make_shared<CompilerContextObject>();
  cc->source = source;
  addBuiltInTypes(cc->typeTable);
  return cc;
}
