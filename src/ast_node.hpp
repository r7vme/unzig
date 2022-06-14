#pragma once

#include <memory>
#include <string>

#include "codegen.hpp"
#include "dotgen.hpp"

// Inspired by https://www.foonathan.net/2020/01/type-erasure/
class AstNode {
  class Base {
  public:
    virtual ~Base() = default;
    virtual std::unique_ptr<Base> clone() const = 0;
    virtual bool isEqual(const Base &other) const = 0;
    virtual llvm::Value *codegen(CodeGenerator *g) const = 0;
    virtual void dotgen(DotGenerator *g) const = 0;
  };

  template <typename T> class Wrapper final : public Base {
  public:
    Wrapper(T obj) : obj(std::move(obj)) {}
    std::unique_ptr<Base> clone() const override {
      return std::make_unique<Wrapper<T>>(obj);
    }

    bool isEqual(const Base &other) const {
      return obj.isEqual(static_cast<const Wrapper<T> &>(other).obj);
    };

    llvm::Value *codegen(CodeGenerator *g) const override {
      return obj.codegen(g);
    };

    void dotgen(DotGenerator *g) const override { return obj.dotgen(g); }

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

  bool operator==(const AstNode &other) const {
    return ptr->isEqual(*other.ptr);
  }

  llvm::Value *codegen(CodeGenerator *g) const { return ptr->codegen(g); };

  void dotgen(DotGenerator *g) const { return ptr->dotgen(g); }

private:
  std::unique_ptr<Base> ptr;
};
