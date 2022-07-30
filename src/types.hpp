#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

enum class UzTypeId { Void, Int, Float, INVALID };

struct UzTypeInt {
  uint32_t bitNum;
  bool isSigned;
};

struct UzTypeFloat {
  uint32_t bitNum;
};

using AnyUzType = std::variant<UzTypeInt, UzTypeFloat>;

struct UzType {
  UzTypeId id{UzTypeId::INVALID};
  std::string name;
  AnyUzType type;
};

struct TypeTable {
  std::unordered_map<std::string, UzType> table;

  std::optional<UzType> findType(const std::string &name) {
    auto it = table.find(name);
    if (it != table.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  void addType(const UzType& type)
  {
    table.insert({type.name, type});
  }
};

inline void addBuiltInTypes(TypeTable &t) {
  t.addType({
      .id = UzTypeId::Int,
      .name = "i32",
      .type = UzTypeInt{32, false},
  });
  t.addType({
      .id = UzTypeId::Float,
      .name = "f32",
      .type = UzTypeInt{32, false},
  });
}
