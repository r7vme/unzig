#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

struct Symbol;
struct ScopeObject;

using Scope = std::shared_ptr<ScopeObject>;
using SymbolTable = std::unordered_map<std::string, Symbol>;

enum class SymbolType {
  Var,
  Fn,
};

struct Symbol {
  std::string name;
  SymbolType type;
  // data
  Symbol(const std::string &name, SymbolType type) : name(name), type(type) {}
};

class ScopeObject {
  SymbolTable table{};
  Scope parent{nullptr};

public:
  void insertSymbol(const Symbol &symbol) {
    table.insert({symbol.name, symbol});
  }

  std::optional<Symbol> lookupSymbol(const std::string &name) {
    auto it = table.find(name);
    if (it != table.end()) {
      return it->second;
    }
    if (parent) {
      return parent->lookupSymbol(name);
    }
    return std::nullopt;
  }
  void setParent(const Scope scope) { parent = scope; }
};
