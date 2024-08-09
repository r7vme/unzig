#pragma once

#include "types.hpp"
#include <llvm/IR/Instructions.h>
#include <memory>
#include <string>

enum class SymbolType {
  Var,
  Fn,
};

struct SymbolObject {
  SymbolType symbolType;
  std::string name;
  bool isGlobal;

  // var specific
  UzType dataType;
  llvm::AllocaInst *allocaInst;

  // fn specific
  uint16_t param_num;
};

using Symbol = std::shared_ptr<SymbolObject>;

inline Symbol createSymbol(SymbolType symbolType, const std::string &name, const UzType dataType,
                           const bool isGlobal, const uint16_t param_num) {
  auto symbol = std::make_shared<SymbolObject>();
  symbol->symbolType = symbolType;
  symbol->name = name;
  symbol->isGlobal = isGlobal;
  symbol->dataType = dataType;
  symbol->allocaInst = nullptr;
  symbol->param_num = param_num;
  return symbol;
}
