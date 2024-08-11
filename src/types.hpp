#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

enum class UzTypeId { VOID, I8, I16, I32, U8, U16, U32, F32, INVALID };

struct UzType {
  UzTypeId id{UzTypeId::INVALID};
  // XXX: placeholder for future complex types
};

using UzTypePtr = std::shared_ptr<const UzType>;

inline bool operator==(const UzTypePtr &lhs, const UzTypePtr &rhs) {
  return lhs.get() == rhs.get();
}

inline bool isFloatType(const UzTypePtr type) {
  assert(type);
  return type->id == UzTypeId::F32;
}

inline bool isIntegerType(const UzTypePtr type) {
  assert(type);
  return type->id == UzTypeId::F32;
}

class TypeTable {
protected:
  TypeTable() {
    addType("bool", UzType{.id = UzTypeId::I32});
    addType("i8", UzType{.id = UzTypeId::I8});
    addType("i16", UzType{.id = UzTypeId::I16});
    addType("i32", UzType{.id = UzTypeId::I32});
    addType("u8", UzType{.id = UzTypeId::U8});
    addType("u16", UzType{.id = UzTypeId::U16});
    addType("u32", UzType{.id = UzTypeId::U32});
    addType("f32", UzType{.id = UzTypeId::F32});
  }

public:
  TypeTable(const TypeTable &) = delete;
  TypeTable &operator=(const TypeTable &) = delete;

  static TypeTable &getInstance() {
    static TypeTable typeTable;
    return typeTable;
  }

  UzTypePtr findType(const std::string &name) const {
    auto it = table_.find(name);
    if (it != table_.end()) {
      return it->second;
    }
    return nullptr;
  }

  void addType(const std::string &name, const UzType &type) {
    if (!table_.insert({name, std::make_shared<UzType>(type)}).second) {
      throw std::runtime_error(std::string("type with the name ") + name + " already exists");
    }
  }

private:
  std::unordered_map<std::string, UzTypePtr> table_;
};
