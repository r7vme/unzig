#pragma once

#include <memory>
#include <string>

#include "ast_equality_comparator.hpp"
#include "codegen.hpp"
#include "dotgen.hpp"
#include "scope.hpp"
#include "types.hpp"

class AstNode {
  class Base {
  public:
    virtual ~Base() = default;
    virtual std::unique_ptr<Base> clone() const = 0;

    // mine
    virtual llvm::Value *codegen(CodeGenerator *g) = 0;
    virtual void dotgen(DotGenerator *g) = 0;
  };

  template <typename T> class Wrapper final : public Base {
  public:
    Wrapper(T obj) : obj(std::move(obj)) {}
    std::unique_ptr<Base> clone() const override {
      return std::make_unique<Wrapper<T>>(obj);
    }

    // mine
    llvm::Value *codegen(CodeGenerator *g) override { return obj.codegen(g); };
    void dotgen(DotGenerator *g) override { return obj.dotgen(g); }

  private:
    T obj;
  };

public:
  template <typename T>
  AstNode(T obj) : ptr(std::make_unique<Wrapper<T>>(std::move(obj))) {}
  AstNode(const AstNode &other) : ptr(other.ptr->clone()) {}
  AstNode &operator=(const AstNode other) {
    ptr = other.ptr->clone();
    return *this;
  }

  // mine
  llvm::Value *codegen(CodeGenerator *g) { return ptr->codegen(g); };
  void dotgen(DotGenerator *g) { return ptr->dotgen(g); }

private:
  std::unique_ptr<Base> ptr;
};
