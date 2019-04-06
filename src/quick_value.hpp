#pragma once

#include <string>

struct QuickValue {
  const std::string type;
  const std::string amount;
  const std::string icon;

  template <typename A, typename B, typename C>
  QuickValue(A && type,
             B && amount,
             C && icon)
      : type{std::forward<A>(type)},
        amount{std::forward<B>(amount)},
        icon{std::forward<C>(icon)} {}

  inline std::string json() const {
    return R"({"type":")" + type
           + R"(","amount":)" + amount
           + R"(,"icon":")" + icon
           + "\"}";
  }

  inline bool operator==(const QuickValue & rhs) const {
    return type == rhs.type && amount == rhs.amount && icon == rhs.icon;
  }

  inline bool operator!=(const QuickValue & rhs) const {
    return !(rhs == *this);
  }

  friend std::ostream & operator<<(std::ostream & stream,
                                   const QuickValue & value) {
    stream << R"({"type":")" << value.type
           << R"(","amount":)" << value.amount
           << R"(,"icon":)" << value.icon
           << '}';
    return stream;
  }
};
