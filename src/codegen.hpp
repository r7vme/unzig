#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

using Code = llvm::Value;

class IntegerExprNode;
class BinaryExprNode;

class CodeGenerator {
  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule;
  llvm::IRBuilder<> llvmIRBuilder;

public:
  CodeGenerator()
      : llvmCtxt(), llvmModule("unzig", llvmCtxt), llvmIRBuilder(llvmCtxt) {}

  Code *generate(IntegerExprNode *astNode);
  Code *generate(BinaryExprNode *astNode);
};
