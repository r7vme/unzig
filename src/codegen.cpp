#include "codegen.hpp"
#include "ast.hpp"
#include "ast_node.hpp"
#include "context.hpp"
#include "types.hpp"
#include <cassert>
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

Value *createCmpEQToZero(CompilerContext cc, Value *value);
Value *createCmpNEToZero(CompilerContext cc, Value *value);
Value *createLogicalNegation(CompilerContext cc, Value *value);
Value *convertToIfCondition(CompilerContext cc, Value *value);
Type *toLLVMType(const UzType &uzType, LLVMContext &ctxt);
Function *getCurrentFunc(CompilerContext cc);

Function *getCurrentFunc(CompilerContext cc) { return cc->ir.GetInsertBlock()->getParent(); }

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
    auto type = std::get<FloatParams>(uzType->type);
    switch (type.bitNum) {
    case 64:
      return Type::getDoubleTy(ctxt);
    case 32:
      return Type::getFloatTy(ctxt);
    case 16:
      return Type::getHalfTy(ctxt);
    default:
      assert(false);
    }
  }
  assert(false);
}

Value *createCmpEQToZero(CompilerContext cc, Value *value) {
  auto type = value->getType();
  if (type->isIntegerTy()) {
    return cc->ir.CreateICmpEQ(value, ConstantInt::get(type, 0));
  } else if (type->isFloatingPointTy()) {
    return cc->ir.CreateFCmpOEQ(value, ConstantFP::get(type, 0.0));
  }
  assert(false);
  return nullptr;
}

Value *createCmpNEToZero(CompilerContext cc, Value *value) {
  auto type = value->getType();
  if (type->isIntegerTy()) {
    return cc->ir.CreateICmpNE(value, ConstantInt::get(type, 0));
  } else if (type->isFloatingPointTy()) {
    return cc->ir.CreateFCmpUNE(value, ConstantFP::get(type, 0.0));
  }
  assert(false);
  return nullptr;
}

Value *createLogicalNegation(CompilerContext cc, Value *value) {
  return createCmpEQToZero(cc, value);
}

Value *convertToIfCondition(CompilerContext cc, Value *value) {
  return createCmpNEToZero(cc, value);
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

Value *Codegen::generate(const BoolExprNode &astNode) {
  return ConstantInt::get(Type::getInt32Ty(cc->llvmCtxt), (astNode.value) ? 1 : 0);
}

Value *Codegen::generate(const BinExprNode &astNode) {
  auto l = astNode.lhs.codegen(this);
  auto r = astNode.rhs.codegen(this);
  if (!l || !r) {
    return nullptr;
  }

  switch (astNode.type) {
  case BinOpType::ADD:
    return cc->ir.CreateAdd(l, r);
  case BinOpType::SUB:
    return cc->ir.CreateSub(l, r);
  case BinOpType::MUL:
    return cc->ir.CreateMul(l, r);
  case BinOpType::DIV:
    return cc->ir.CreateSDiv(l, r);
  }

  return nullptr;
};

Value *Codegen::generate(const VarDeclNode &astNode) {
  // TODO: global variables
  auto llvmType = toLLVMType(astNode.type, cc->llvmCtxt);
  auto initValue = astNode.initExpr.codegen(this);
  if (!initValue) {
    fatalCodegenError("variable must be initialized", astNode.sourcePos);
  }
  llvm::IRBuilder<> TmpB(&getCurrentFunc(cc)->getEntryBlock(),
                         getCurrentFunc(cc)->getEntryBlock().begin());
  auto alloca = TmpB.CreateAlloca(llvmType, nullptr, astNode.name);
  astNode.symbol->allocaInst = alloca;

  return cc->ir.CreateStore(initValue, alloca);
}

Value *Codegen::generate(const VarExprNode &astNode) {
  assert(astNode.varSymbol);
  if (!astNode.varSymbol->allocaInst) {
    fatalCodegenError("unable to find a variable instruction", astNode.sourcePos);
  }
  return cc->ir.CreateLoad(toLLVMType(astNode.varSymbol->dataType, cc->llvmCtxt),
                           astNode.varSymbol->allocaInst, astNode.name);
}

Value *Codegen::generate(const FnCallExprNode &astNode) {
  auto callee = cc->llvmModule.getFunction(astNode.callee);
  if (!callee) {
    fatalCodegenError("function not declared", astNode.sourcePos);
  }
  auto call = cc->ir.CreateCall(callee);
  if (!call) {
    fatalCodegenError("unable to generate a function call", astNode.sourcePos);
  }
  return call;
}

Value *Codegen::generate(const FnDefNode &astNode) {
  auto funcName = astNode.name;
  auto funcReturnType = toLLVMType(astNode.returnType, cc->llvmCtxt);
  auto funcType = FunctionType::get(funcReturnType, false);
  auto func = Function::Create(funcType, Function::ExternalLinkage, funcName, cc->llvmModule);
  BasicBlock *bb = BasicBlock::Create(cc->llvmCtxt, "entry", func);
  cc->ir.SetInsertPoint(bb);

  if (!astNode.body.codegen(this)) {
    fatalCodegenError("unable to generate a function body", astNode.sourcePos);
  }
  return func;
}

Value *Codegen::generate(const BlockNode &astNode) {
  for (auto &s : astNode.statements) {
    if (!(s.codegen(this))) {
      fatalCodegenError("unable to generate a block statement", astNode.sourcePos);
    }
  }
  return cc->ir.GetInsertBlock();
}

Value *Codegen::generate(const AssignStNode &astNode) {
  if (!astNode.varSymbol->allocaInst) {
    fatalCodegenError("unable to find a variable instruction", astNode.sourcePos);
  }

  auto exprValue = astNode.expr.codegen(this);
  if (!exprValue) {
    fatalCodegenError("unable to generate an assignment expression", astNode.sourcePos);
  }

  return cc->ir.CreateStore(exprValue, astNode.varSymbol->allocaInst);
}

Value *Codegen::generate(const ReturnStNode &astNode) {
  auto returnValue = cc->ir.CreateRet(astNode.expr.codegen(this));
  if (!returnValue) {
    fatalCodegenError("unable to generate a return value", astNode.sourcePos);
  }
  return returnValue;
}

Value *Codegen::generate(const IfStNode &astNode) {
  auto ifCondition = astNode.ifCondition.codegen(this);
  if (!ifCondition) {
    fatalCodegenError("unable to generate if condition", astNode.sourcePos);
  }

  auto thenBB = BasicBlock::Create(cc->llvmCtxt, "", getCurrentFunc(cc));
  auto elseBB = BasicBlock::Create(cc->llvmCtxt, "", getCurrentFunc(cc));
  auto mergeBB = BasicBlock::Create(cc->llvmCtxt, "", getCurrentFunc(cc));
  cc->ir.CreateCondBr(convertToIfCondition(cc, ifCondition), thenBB, elseBB);

  cc->ir.SetInsertPoint(thenBB);
  auto thenBlock = astNode.thenBlock.codegen(this);
  if (!thenBlock) {
    fatalCodegenError("unable to generate then block", astNode.sourcePos);
  }
  cc->ir.CreateBr(mergeBB);

  cc->ir.SetInsertPoint(elseBB);
  auto elseBlock = astNode.elseBlock.codegen(this);
  if (!elseBlock) {
    fatalCodegenError("unable to generate else block", astNode.sourcePos);
  }
  cc->ir.CreateBr(mergeBB);

  cc->ir.SetInsertPoint(mergeBB);
  return cc->ir.GetInsertBlock();
}

Value *Codegen::generate(const EmptyNode &astNode) { return cc->ir.GetInsertBlock(); }

Value *Codegen::generate(const PrefixExprNode &astNode) {
  auto value = astNode.expr.codegen(this);
  if (!value) {
    fatalCodegenError("code generation for the prefix node failed", astNode.sourcePos);
  }

  for (auto op : astNode.operators) {
    switch (op) {
    case PrefixOpType::NOT:
      value = createLogicalNegation(cc, value);
    }
  }

  return value;
}

Value *Codegen::generate(const RootNode &astNode) {
  for (auto &decl : astNode.declarations) {
    if (!decl.codegen(this)) {
      fatalCodegenError("code generation for the root node failed", astNode.sourcePos);
    }
  }
  return cc->llvmModule.getFunction("main");
}
