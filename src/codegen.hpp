#pragma once
#include <llvm/IR/Function.h>
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
class AssignStNode;
class ReturnStNode;
class VarExprNode;
class FnCallExprNode;
class EmptyNode;

class CodeGenerator {
  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule;
  llvm::IRBuilder<> llvmIRBuilder;

  llvm::Function* curFunc;
public:
  CodeGenerator()
      : llvmCtxt(), llvmModule("unzig", llvmCtxt), llvmIRBuilder(llvmCtxt) {}
  const llvm::Module &getLLVMModule() const { return llvmModule; };

  void fatalCodegenError(const std::string& msg);

  llvm::Value *generate(const FloatExprNode &astNode);
  llvm::Value *generate(const IntegerExprNode &astNode);
  llvm::Value *generate(const BinExprNode &astNode);
  llvm::Value *generate(const VarDeclNode &astNode);
  llvm::Value *generate(const FnDefNode &astNode);
  llvm::Value *generate(const BlockNode &astNode);
  llvm::Value *generate(const RootNode &astNode);
  llvm::Value *generate(const AssignStNode &astNode);
  llvm::Value *generate(const ReturnStNode &astNode);
  llvm::Value *generate(const VarExprNode &astNode);
  llvm::Value *generate(const FnCallExprNode &astNode);
  llvm::Value *generate(const EmptyNode &astNode);
};
