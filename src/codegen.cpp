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
#include <optional>

using llvm::BasicBlock;
using llvm::ConstantFP;
using llvm::ConstantInt;
using llvm::Function;
using llvm::FunctionType;
using llvm::LLVMContext;
using llvm::PHINode;
using llvm::Type;
using llvm::Value;

Type *toLLVMType(const UzType &uzType, LLVMContext &ctxt) {
  if (uzType->id == UzTypeId::Void) {
    return Type::getVoidTy(ctxt);
  } else if (uzType->id == UzTypeId::Int) {
    auto type = std::get<IntParams>(uzType->type);
    switch (type.bitNum) {
    case 64:
      return Type::getInt64Ty(ctxt);
    case 32:
      return Type::getInt32Ty(ctxt);
    case 16:
      return Type::getInt16Ty(ctxt);
    case 8:
      return Type::getInt8Ty(ctxt);
    default:
      assert(false);
    }
  } else if (uzType->id == UzTypeId::Float) {
    return Type::getFloatTy(ctxt);
  }
  assert(false);
}

void Codegen::fatalCodegenError(const std::string &msg, const size_t sourcePos) {
  auto hightlightedLine = cc->source->getHightlightedPosition(sourcePos);
  std::cerr << "Codegen error: " << msg << '\n' << hightlightedLine << std::endl;
  std::exit(EXIT_FAILURE);
}

Value *Codegen::generate(const FloatExprNode &astNode) {
  return ConstantFP::get(Type::getFloatTy(cc->llvmCtxt), astNode.value);
}

Value *Codegen::generate(const IntegerExprNode &astNode) {
  const int32_t decimal = 10;
  return ConstantInt::get(Type::getInt32Ty(cc->llvmCtxt), astNode.value, decimal);
}

Value *Codegen::generate(const BinExprNode &astNode) {
  auto *l = astNode.lhs.codegen(this);
  auto *r = astNode.rhs.codegen(this);
  if (!l || !r) {
    return nullptr;
  }

  switch (astNode.type) {
  case BinOpType::ADD:
    return cc->llvmIRBuilder.CreateAdd(l, r);
  case BinOpType::SUB:
    return cc->llvmIRBuilder.CreateSub(l, r);
  case BinOpType::MUL:
    return cc->llvmIRBuilder.CreateMul(l, r);
  case BinOpType::DIV:
    return cc->llvmIRBuilder.CreateSDiv(l, r);
  }

  return nullptr;
};

Value *Codegen::generate(const VarDeclNode &astNode) {
  // TODO: global variables
  auto *llvmType = toLLVMType(astNode.type, cc->llvmCtxt);
  auto *initValue = astNode.initExpr.codegen(this);
  if (!initValue) {
    fatalCodegenError("variable must be initialized", astNode.sourcePos);
  }
  llvm::IRBuilder<> TmpB(&cc->curFunc->getEntryBlock(), cc->curFunc->getEntryBlock().begin());
  auto *alloca = TmpB.CreateAlloca(llvmType, nullptr, astNode.name);
  astNode.symbol->allocaInst = alloca;

  return cc->llvmIRBuilder.CreateStore(initValue, alloca);
}

Value *Codegen::generate(const VarExprNode &astNode) {
  if (!astNode.varSymbol->allocaInst) {
    fatalCodegenError("unable to find a variable instruction", astNode.sourcePos);
  }
  return cc->llvmIRBuilder.CreateLoad(toLLVMType(astNode.varSymbol->dataType, cc->llvmCtxt),
                                      astNode.varSymbol->allocaInst, astNode.name);
}

Value *Codegen::generate(const FnCallExprNode &astNode) {
  auto *callee = cc->llvmModule.getFunction(astNode.callee);
  if (!callee) {
    fatalCodegenError("function not declared", astNode.sourcePos);
  }
  auto *call = cc->llvmIRBuilder.CreateCall(callee);
  if (!call) {
    fatalCodegenError("unable to generate a function call", astNode.sourcePos);
  }
  return call;
}

Value *Codegen::generate(const FnDefNode &astNode) {
  auto funcName = astNode.name;
  auto *funcReturnType = toLLVMType(astNode.returnType, cc->llvmCtxt);
  auto *funcType = FunctionType::get(funcReturnType, false);
  auto *func = Function::Create(funcType, Function::ExternalLinkage, funcName, cc->llvmModule);
  cc->curFunc = func;
  BasicBlock *bb = BasicBlock::Create(cc->llvmCtxt, "entry", cc->curFunc);
  cc->llvmIRBuilder.SetInsertPoint(bb);

  if (!astNode.body.codegen(this)) {
    fatalCodegenError("unable to generate a function body", astNode.sourcePos);
  }
  return func;
}

Value *Codegen::generate(const BlockNode &astNode) {
  Value * ret = nullptr;
  for (auto &s : astNode.statements) {
    if (!(ret = s.codegen(this))) {
      fatalCodegenError("unable to generate a block statement", astNode.sourcePos);
    }
  }
  return ret;
}

Value *Codegen::generate(const AssignStNode &astNode) {
  if (!astNode.varSymbol->allocaInst) {
    fatalCodegenError("unable to find a variable instruction", astNode.sourcePos);
  }

  auto *exprValue = astNode.expr.codegen(this);
  if (!exprValue) {
    fatalCodegenError("unable to generate an assignment expression", astNode.sourcePos);
  }

  return cc->llvmIRBuilder.CreateStore(exprValue, astNode.varSymbol->allocaInst);
}

Value *Codegen::generate(const ReturnStNode &astNode) {
  auto *returnValue = cc->llvmIRBuilder.CreateRet(astNode.expr.codegen(this));
  if (!returnValue) {
    fatalCodegenError("unable to generate a return value", astNode.sourcePos);
  }
  return returnValue;
}

Value *Codegen::generate(const IfStNode &astNode) {
  //  auto *ifCondition = astNode.ifCondition.codegen(this);
  //  if (!ifCondition) {
  //    fatalCodegenError("unable to generate if statement code", astNode.sourcePos);
  //  }

  // stub condition
  auto *val1 = ConstantInt::get(Type::getInt32Ty(cc->llvmCtxt), 0);
  auto *val2 = ConstantInt::get(Type::getInt32Ty(cc->llvmCtxt), 0);
  auto *ifCondition = cc->llvmIRBuilder.CreateICmpNE(val1, val2);

  auto *thenBB = BasicBlock::Create(cc->llvmCtxt, "", cc->curFunc);
  auto *elseBB = BasicBlock::Create(cc->llvmCtxt, "", cc->curFunc);
  auto *mergeBB = BasicBlock::Create(cc->llvmCtxt, "", cc->curFunc);
  cc->llvmIRBuilder.CreateCondBr(ifCondition, thenBB, elseBB);

  cc->llvmIRBuilder.SetInsertPoint(thenBB);
  auto *thenBlock = astNode.thenBlock.codegen(this);
  if (!thenBlock) {
    fatalCodegenError("unable to generate if statement code 1", astNode.sourcePos);
  }
  cc->llvmIRBuilder.CreateBr(mergeBB);

  cc->llvmIRBuilder.SetInsertPoint(elseBB);
  auto *elseBlock = astNode.elseBlock.codegen(this);
  if (!elseBlock) {
    fatalCodegenError("unable to generate if statement code 2", astNode.sourcePos);
  }
  cc->llvmIRBuilder.CreateBr(mergeBB);

  cc->llvmIRBuilder.SetInsertPoint(mergeBB);
  return cc->llvmIRBuilder.GetInsertBlock();
}

Value *Codegen::generate(const EmptyNode &astNode) { return nullptr; }

Value *Codegen::generate(const RootNode &astNode) {
  for (auto &decl : astNode.declarations) {
    if (!decl.codegen(this)) {
      fatalCodegenError("code generation for the root node failed", astNode.sourcePos);
    }
  }
  return cc->llvmModule.getFunction("main");
}
