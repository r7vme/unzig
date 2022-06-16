#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

struct SymbolObject;
struct ScopeObject;

using Scope = std::shared_ptr<ScopeObject>;
using Symbol = std::shared_ptr<SymbolObject>;
using SymbolTable = std::unordered_map<std::string, Symbol>;

enum class SymbolType {
  Var,
  Fn,
};

struct SymbolObject {
  std::string name;
  SymbolType type;
  // data
  SymbolObject(const std::string &name, SymbolType type) : name(name), type(type) {}
};

class ScopeObject {
  SymbolTable table{};
  Scope parent{nullptr};

public:
  void insertSymbol(const Symbol &symbol) {
    table.insert({symbol->name, symbol});
  }

  std::optional<Symbol> lookupSymbol(const std::string &name) const {
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
