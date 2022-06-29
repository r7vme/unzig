#pragma once
#include "source.hpp"
#include "types.hpp"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>

struct CompilerContextObject {
  Source source;
  TypeTable typeTable;
  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule;
  llvm::IRBuilder<> llvmIRBuilder;
  llvm::Function *curFunc;

  CompilerContextObject(const Source source)
      : source(source), llvmCtxt(), llvmModule("unzig", llvmCtxt), llvmIRBuilder(llvmCtxt) {}
};

using CompilerContext = std::shared_ptr<CompilerContextObject>;

inline CompilerContext createCompilerContext(const Source source) {
  return std::make_shared<CompilerContextObject>(source);
}
