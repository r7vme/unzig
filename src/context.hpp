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
  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule;
  llvm::IRBuilder<> ir;

  Source source;
  TypeTable typeTable;

  CompilerContextObject() : llvmCtxt(), llvmModule("unzig", llvmCtxt), ir(llvmCtxt) {}
};

using CompilerContext = std::shared_ptr<CompilerContextObject>;

CompilerContext createCompilerContext(const Source source);
