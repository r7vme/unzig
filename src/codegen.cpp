#include "codegen.hpp"
#include "ast.hpp"
#include <cstdint>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

llvm::Value *CodeGenerator::generate(FloatExprNode *astNode) {
  return llvm::ConstantFP::get(llvm::Type::getFloatTy(llvmCtxt),
                               astNode->value);
}

llvm::Value *CodeGenerator::generate(IntegerExprNode *astNode) {
  const int32_t decimal = 10;
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvmCtxt),
                                astNode->value, decimal);
}

llvm::Value *CodeGenerator::generate(BinExprNode *astNode) {
  auto *l = astNode->lhs->codegen(this);
  auto *r = astNode->rhs->codegen(this);
  if (!l || !r) {
    return nullptr;
  }

  switch (astNode->type) {
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
