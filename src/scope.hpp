#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "symbol.hpp"

class ScopeObject;
using Scope = std::shared_ptr<ScopeObject>;

Scope createChildScope(const Scope parent);

class ScopeObject {
  using SymbolTable = std::unordered_map<std::string, Symbol>;
  SymbolTable table;
  Scope parent;

public:
  ScopeObject(const Scope parent);

  bool isGlobal{false};
  void insertSymbol(const Symbol &symbol);
  std::optional<Symbol> lookupSymbol(const std::string &name) const;
};
