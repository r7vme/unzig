#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

enum class UzTypeId { Void, Int, Float, SIZE };

struct UzTypeInt {
  uint32_t bitNum;
  bool isSigned;
};

struct UzTypeFloat {
  uint32_t bitNum;
};

using AnyUzType = std::variant<UzTypeInt, UzTypeFloat>;

struct UzType {
  UzTypeId id;
  std::string name;

  AnyUzType type;
};

bool operator==(const UzType &lhs, const UzType &rhs);

std::optional<UzType> toUzType(const std::string &input);

using TypeTable = std::unordered_map<std::string, UzType>;
