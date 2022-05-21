#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

class FloatExprNode;
class IntegerExprNode;
class BinExprNode;
class VarDeclNode;
class FnDefNode;
class BlockNode;
class RootNode;

class CodeGenerator {
  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule;
  llvm::IRBuilder<> llvmIRBuilder;

public:
  CodeGenerator()
      : llvmCtxt(), llvmModule("unzig", llvmCtxt), llvmIRBuilder(llvmCtxt) {}

  llvm::Value *generate(FloatExprNode *astNode);
  llvm::Value *generate(IntegerExprNode *astNode);
  llvm::Value *generate(BinExprNode *astNode);
  llvm::Value *generate(VarDeclNode *astNode);
  llvm::Value *generate(FnDefNode *astNode);
  llvm::Value *generate(BlockNode *astNode);
  llvm::Value *generate(RootNode *astNode);
};
