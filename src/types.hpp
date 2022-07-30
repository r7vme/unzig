#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

enum class UzTypeId { Void, Int, Float, INVALID };

struct IntParams {
  uint32_t bitNum;
  bool isSigned;
};

struct FloatParams {
  uint32_t bitNum;
};

using TypeParams = std::variant<IntParams, FloatParams>;

struct UzTypeObject {
  UzTypeId id{UzTypeId::INVALID};
  std::string name;
  TypeParams type;
};

using UzType = std::shared_ptr<UzTypeObject>;

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
    table.insert({type->name, type});
  }
};

inline void addBuiltInTypes(TypeTable &t) {
  t.addType(std::make_shared<UzTypeObject>(UzTypeObject{
      .id = UzTypeId::Int,
      .name = "i32",
      .type = IntParams{32, false},
  }));
  t.addType(std::make_shared<UzTypeObject>(UzTypeObject{
      .id = UzTypeId::Float,
      .name = "f32",
      .type = FloatParams{32},
  }));
}
