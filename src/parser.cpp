#include "tokenizer.hpp"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include <iostream>

#include "parser.hpp"

static std::unique_ptr<llvm::LLVMContext> llvmCxt;
static std::unique_ptr<llvm::Module> llvmModule;
static std::unique_ptr<llvm::IRBuilder<>> llvmIRBuilder;

llvm::Value *IntegerExprNode::codegen() {
  return llvm::ConstantInt::get(*llvmCxt, llvm::APInt(32, value, 10));
}

llvm::Value *BinaryExprNode::codegen() {
  auto *l = lhs->codegen();
  auto *r = rhs->codegen();
  if (!l || !r) {
    return nullptr;
  }

  switch (type) {
  case BinaryOpType::ADD:
    return llvmIRBuilder->CreateAdd(l, r);
  case BinaryOpType::SUB:
    return llvmIRBuilder->CreateSub(l, r);
  case BinaryOpType::MUL:
    return llvmIRBuilder->CreateMul(l, r);
  case BinaryOpType::DIV:
    return llvmIRBuilder->CreateSDiv(l, r);
  }

  return nullptr;
}

std::shared_ptr<AstNode> LogError(const std::string &str) {
  std::cerr << "error: " << str << std::endl;
  return nullptr;
}

llvm::Value *LogErrorV(const std::string &str) {
  LogError(str);
  return nullptr;
}

// TODO: actually implement parser
AstNodePtr parse(Tokens tokens)
{
  llvmCxt = std::make_unique<llvm::LLVMContext>();
  llvmModule = std::make_unique<llvm::Module>("unzig", *llvmCxt);
  llvmIRBuilder = std::make_unique<llvm::IRBuilder<>>(*llvmCxt);

  return std::make_shared<BinaryExprNode>(
    BinaryOpType::MUL,
    std::make_shared<IntegerExprNode>("1"),
    std::make_shared<IntegerExprNode>("2")
  );
}
