#include "codegen.hpp"
#include "ast.hpp"
#include "ast_node.hpp"
#include "types.hpp"
#include <cstdint>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
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

void CodeGenerator::fatalCodegenError(const std::string &msg, const size_t sourcePos) {
  auto hightlightedLine = source->getHightlightedPosition(sourcePos);
  std::cerr << "Syntax error: " << msg << '\n' << hightlightedLine << std::endl;
  std::exit(EXIT_FAILURE);
}

Value *CodeGenerator::generate(const FloatExprNode &astNode) {
  return ConstantFP::get(Type::getFloatTy(llvmCtxt), astNode.value);
}

Value *CodeGenerator::generate(const IntegerExprNode &astNode) {
  const int32_t decimal = 10;
  return ConstantInt::get(Type::getInt32Ty(llvmCtxt), astNode.value, decimal);
}

Value *CodeGenerator::generate(const BinExprNode &astNode) {
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

Value *CodeGenerator::generate(const VarDeclNode &astNode) {
  // TODO: global variables
  auto *llvmType = toLLVMType(astNode.type, llvmCtxt);
  auto *initValue = astNode.initExpr.codegen(this);
  if (!initValue) {
    fatalCodegenError("variable must be initialized", astNode.sourcePos);
  }
  llvm::IRBuilder<> TmpB(&curFunc->getEntryBlock(),
                         curFunc->getEntryBlock().begin());
  auto *alloca = TmpB.CreateAlloca(llvmType, nullptr, astNode.name);

  return llvmIRBuilder.CreateStore(initValue, alloca);
}

Value *CodeGenerator::generate(const VarExprNode &astNode) { return nullptr; }

Value *CodeGenerator::generate(const FnCallExprNode &astNode) {
  auto *callee = llvmModule.getFunction(astNode.callee);
  if (!callee) {
    fatalCodegenError("function not declared", astNode.sourcePos);
  }
  auto* call = llvmIRBuilder.CreateCall(callee, nullptr, "calltmp");
  if (!call)
  {
    fatalCodegenError("HUJ", astNode.sourcePos);
  }
  return call;
}

Value *CodeGenerator::generate(const FnDefNode &astNode) {
  auto funcName = astNode.name;
  auto *funcReturnType = toLLVMType(astNode.returnType, llvmCtxt);
  auto *funcType = FunctionType::get(funcReturnType, false);
  auto *func = Function::Create(funcType, Function::ExternalLinkage, funcName,
                                llvmModule);
  curFunc = func;
  astNode.body.codegen(this);
  return func;
}

Value *CodeGenerator::generate(const BlockNode &astNode) {
  BasicBlock *bb = BasicBlock::Create(llvmCtxt, "entry", curFunc);
  llvmIRBuilder.SetInsertPoint(bb);

  for (auto &s : astNode.statements) {
    if (!(s.codegen(this))) {
      return nullptr;
    }
  }

  return bb;
}

Value *CodeGenerator::generate(const AssignStNode &astNode) { return nullptr; }

Value *CodeGenerator::generate(const ReturnStNode &astNode) {
  return llvmIRBuilder.CreateRet(astNode.expr.codegen(this));
}

Value *CodeGenerator::generate(const EmptyNode &astNode) { return nullptr; }

Value *CodeGenerator::generate(const RootNode &astNode) {
  for (auto &decl : astNode.declarations) {
    if (!decl.codegen(this)) {
      fatalCodegenError("code generation for the root node failed", astNode.sourcePos);
    }
  }
  return llvmModule.getFunction("main");
}
