#include "tokenizer.hpp"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include <cstdint>
#include <iostream>
#include <llvm/IR/Instruction.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>

static std::unique_ptr<llvm::LLVMContext> llvmCxt;
static std::unique_ptr<llvm::Module> llvmModule;
static std::unique_ptr<llvm::IRBuilder<>> llvmIRBuilder;

class Expr {
public:
  virtual ~Expr() = default;
  virtual llvm::Value *codegen() = 0;
};

using ExprPtr = std::shared_ptr<Expr>;

std::shared_ptr<Expr> LogError(const std::string& str) {
  std::cerr << "error: " << str << std::endl;
  return nullptr;
}

llvm::Value *LogErrorV(const std::string& str) {
  LogError(str);
  return nullptr;
}

class VarExpr : public Expr {
  const std::string name;

public:
  VarExpr(const std::string &name) : name(name) {}

  llvm::Value *codegen() override {
    return nullptr;
  };
};

class IntegerExpr : public Expr {
  const std::string value;

public:
  IntegerExpr(const std::string &value) : value(value) {}

  llvm::Value *codegen() override {
    return llvm::ConstantInt::get(*llvmCxt, llvm::APInt(32, value, 10));
  };
};

enum BinaryOpType { ADD, SUB, MUL, DIV };

class BinaryExpr : public Expr {
  BinaryOpType type;
  ExprPtr lhs;
  ExprPtr rhs;

public:
  BinaryExpr(const BinaryOpType type, const ExprPtr lhs, const ExprPtr rhs)
      : type(type), lhs(lhs), rhs(rhs) {}

  llvm::Value *codegen() override {
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
  };
};

int main() {
  llvmCxt = std::make_unique<llvm::LLVMContext>();
  llvmModule = std::make_unique<llvm::Module>("unzig", *llvmCxt);
  llvmIRBuilder = std::make_unique<llvm::IRBuilder<>>(*llvmCxt);

  // 123 + (1 * 2);
  ExprPtr onePlusTwo = std::make_shared<BinaryExpr>(
    BinaryOpType::MUL,
    std::make_shared<IntegerExpr>("1"),
    std::make_shared<IntegerExpr>("2")
  );
  ExprPtr expr = std::make_shared<BinaryExpr>(
    BinaryOpType::ADD,
    std::make_shared<IntegerExpr>("123"),
    onePlusTwo
  );

  if (auto *code = expr->codegen()) {
    std::cout << "==========" << std::endl;
    code->print(llvm::errs());
    std::cout << "\n==========" << std::endl;
  }

  return 0;
}
