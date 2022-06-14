#pragma once

#include <memory>
#include <string>
#include <unordered_map>

struct Symbol;
struct Scope;
using SymbolTable = std::unordered_map<std::string, Symbol>;
using ScopePtr = std::shared_ptr<Scope>;

enum class SymbolType {
  Var,
};

struct Symbol {
  SymbolType type;
  std::string name;
  // data
};

struct Scope {
  ScopePtr parentScope;
  SymbolTable symbolTable;

  void insert(const Symbol &symbol){};
  bool lookup(const std::string &name) { return false; };
};
