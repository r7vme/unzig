#pragma once
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <string>
#include "tokenizer.hpp"

class AstNode {
public:
  virtual ~AstNode() = default;
  virtual llvm::Value *codegen() = 0;
};

using AstNodePtr = std::shared_ptr<AstNode>;

class IntegerExprNode : public AstNode {
  const std::string value;

public:
  IntegerExprNode(const std::string &value) : value(value) {}
  llvm::Value *codegen() override;
};

enum BinaryOpType { ADD, SUB, MUL, DIV };

class BinaryExprNode : public AstNode {
  BinaryOpType type;
  AstNodePtr lhs;
  AstNodePtr rhs;

public:
  BinaryExprNode(const BinaryOpType type, const AstNodePtr lhs, const AstNodePtr rhs)
      : type(type), lhs(lhs), rhs(rhs) {}
  llvm::Value *codegen() override;
};

AstNodePtr parse(Tokens tokens);
