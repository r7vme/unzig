#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

using Code = llvm::Value;

class FloatExprNode;
class IntegerExprNode;
class BinExprNode;
class VarDeclNode;
class FnDefNode;
class BlockNode;

class CodeGenerator {
  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule;
  llvm::IRBuilder<> llvmIRBuilder;

public:
  CodeGenerator()
      : llvmCtxt(), llvmModule("unzig", llvmCtxt), llvmIRBuilder(llvmCtxt) {}

  Code *generate(FloatExprNode *astNode);
  Code *generate(IntegerExprNode *astNode);
  Code *generate(BinExprNode *astNode);
  Code *generate(VarDeclNode *astNode);
  Code *generate(FnDefNode *astNode);
  Code *generate(BlockNode *astNode);
};
