#include "tokenizer.hpp"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include <iostream>
#include <memory>
#include <optional>

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

class ParserCtx {
  Tokens tokens;
  size_t cursor{0};

public:
  ParserCtx(const Tokens &tokens) : tokens(tokens){};
  Token getNextToken() { return tokens[cursor++]; }
};

// NumberExpr <- FLOAT / INTEGER
AstNodePtr parseNumberExpr(ParserCtx &ctx) {
  auto token = ctx.getNextToken();

  if (token.id == TokenId::IntegerLiteral) {
    return std::make_shared<IntegerExprNode>(token.value);
  };
  return LogError("can not parse token");
}

// PrimaryExpr <- GroupedExpr
//             / FnCallExpr
//             / VarExpr
//             / NumberExpr
AstNodePtr parsePrimaryExpr(ParserCtx &ctx) {
  // TODO: GroupedExpr / FnCallExpr / VarExpr
  return parseNumberExpr(ctx);
}

bool isBinaryOp(const Token &token) {
  return (token.id == TokenId::Asterisk) || (token.id == TokenId::Colon) ||
         (token.id == TokenId::Plus) || (token.id == TokenId::Minus);
}

std::optional<BinaryOpType> mayBeToBinaryOpType(const Token &token) {
  switch (token.id) {
  case TokenId::Asterisk:
    return BinaryOpType::MUL;
  case TokenId::Slash:
    return BinaryOpType::DIV;
  case TokenId::Plus:
    return BinaryOpType::ADD;
  case TokenId::Minus:
    return BinaryOpType::SUB;
  default:
    return std::nullopt;
  }
}

// Expr <- PrimaryExpr (BinaryOp PrimaryExpr)*
AstNodePtr parseExpr(ParserCtx &ctx) {
  auto lhs = parsePrimaryExpr(ctx);
  if (!lhs) {
    return nullptr;
  }

  auto binaryOp = mayBeToBinaryOpType(ctx.getNextToken());
  if (!binaryOp) {
    return lhs;
  }

  auto rhs = parsePrimaryExpr(ctx);
  if (!rhs) {
    return nullptr;
  }

  return std::make_shared<BinaryExprNode>(binaryOp.value(), lhs, rhs);
}

// TODO: actually implement parser
AstNodePtr parse(Tokens tokens) {
  llvmCxt = std::make_unique<llvm::LLVMContext>();
  llvmModule = std::make_unique<llvm::Module>("unzig", *llvmCxt);
  llvmIRBuilder = std::make_unique<llvm::IRBuilder<>>(*llvmCxt);

  ParserCtx ctx(tokens);

  return parseExpr(ctx);
}
