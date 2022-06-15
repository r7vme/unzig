#include "codegen.hpp"
#include "ast.hpp"
#include "types.hpp"
#include <cstdint>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

using llvm::BasicBlock;
using llvm::ConstantFP;
using llvm::ConstantInt;
using llvm::Function;
using llvm::FunctionType;
using llvm::LLVMContext;
using llvm::Type;
using llvm::Value;

Type *toLLVMType(const UzType &uzType, LLVMContext &ctxt) {
  switch (uzType.id) {
  case UzTypeId::Void:
    return Type::getVoidTy(ctxt);
  case UzTypeId::Int32:
    return Type::getInt32Ty(ctxt);
  case UzTypeId::Float32:
    return Type::getFloatTy(ctxt);
  default:
    assert(false);
  }
  assert(false);
  return nullptr;
}

Value *CodeGenerator::generate(const FloatExprNode & astNode) {
  return ConstantFP::get(Type::getFloatTy(llvmCtxt), astNode.value);
}

Value *CodeGenerator::generate(const IntegerExprNode & astNode) {
  const int32_t decimal = 10;
  return ConstantInt::get(Type::getInt32Ty(llvmCtxt), astNode.value, decimal);
}

Value *CodeGenerator::generate(const BinExprNode & astNode) {
  auto *l = astNode.lhs.codegen(this);
  auto *r = astNode.rhs.codegen(this);
  if (!l || !r) {
    return nullptr;
  }

  switch (astNode.type) {
  case BinOpType::ADD:
    return llvmIRBuilder.CreateAdd(l, r);
  case BinOpType::SUB:
    return llvmIRBuilder.CreateSub(l, r);
  case BinOpType::MUL:
    return llvmIRBuilder.CreateMul(l, r);
  case BinOpType::DIV:
    return llvmIRBuilder.CreateSDiv(l, r);
  }

  return nullptr;
};

Value *CodeGenerator::generate(const VarDeclNode & astNode) { return nullptr; }
Value *CodeGenerator::generate(const VarExprNode & astNode) { return nullptr; }
Value *CodeGenerator::generate(const FnCallExprNode & astNode) { return nullptr; }

Value *CodeGenerator::generate(const FnDefNode & astNode) {
  auto funcName = astNode.name;
  auto *funcReturnType = toLLVMType(astNode.returnType, llvmCtxt);
  auto *funcType = FunctionType::get(funcReturnType, false);
  auto *func = Function::Create(funcType, Function::ExternalLinkage, funcName,
                                llvmModule);

  BasicBlock *entryBlock = BasicBlock::Create(llvmCtxt, "entry", func);
  llvmIRBuilder.SetInsertPoint(entryBlock);
  llvmIRBuilder.CreateRet(nullptr);

  // TODO block codegen
  // if (Value *RetVal = astNode.body.codegen()) {
  //   llvmIRBuilder.CreateRet(RetVal);
  // }
  // else {
  // func->eraseFromParent();
  // return nullptr;
  // }

  return func;
}

Value *CodeGenerator::generate(const BlockNode & astNode) { return nullptr; }
Value *CodeGenerator::generate(const AssignStNode & astNode) { return nullptr; }
Value *CodeGenerator::generate(const ReturnStNode & astNode) { return nullptr; }
Value *CodeGenerator::generate(const EmptyNode & astNode) { return nullptr; }

Value *CodeGenerator::generate(const RootNode & astNode) {
  for (auto &decl : astNode.declarations) {
    if (!(decl.codegen(this))) {
      return nullptr;
    }
  }
  // return a pointer to required main function
  return llvmModule.getFunction("main");
}
