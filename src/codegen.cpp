#include "codegen.hpp"
#include "ast.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

llvm::Value *CodeGenerator::generate(IntegerExprNode *astNode) {
  return llvm::ConstantInt::get(llvmCtxt, llvm::APInt(32, astNode->value, 10));
}

llvm::Value *CodeGenerator::generate(BinaryExprNode *astNode) {
  auto *l = astNode->lhs->codegen(this);
  auto *r = astNode->rhs->codegen(this);
  if (!l || !r) {
    return nullptr;
  }

  switch (astNode->type) {
  case BinaryOpType::ADD:
    return llvmIRBuilder.CreateAdd(l, r);
  case BinaryOpType::SUB:
    return llvmIRBuilder.CreateSub(l, r);
  case BinaryOpType::MUL:
    return llvmIRBuilder.CreateMul(l, r);
  case BinaryOpType::DIV:
    return llvmIRBuilder.CreateSDiv(l, r);
  }

  return nullptr;
};
