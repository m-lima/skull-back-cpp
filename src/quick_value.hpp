#pragma once

#include <string>

struct QuickValue {
  const std::string type;
  const std::string amount;
  const std::string icon;

  template<typename A, typename B, typename C>
  QuickValue(A && type,
             B && amount,
             C && icon)
      : type{std::forward<A>(type)},
        amount{std::forward<B>(amount)},
        icon{std::forward<C>(icon)} {}

//  template <typename T>
//  QuickValue(T && type,
//             const double amount,
//             T && icon)
//      : type{std::forward<T>(type)},
//        amount{std::to_string(amount)},
//        icon{std::forward<T>(icon)} {}

  inline std::string json() const {
    return R"({"type":")" + type + R"(","amount":)" + amount + R"(,"icon":")" + icon + "\"}";
  }

  inline bool operator==(const QuickValue & rhs) const {
    return type == rhs.type && amount == rhs.amount && icon == rhs.icon;
  }

  inline bool operator!=(const QuickValue & rhs) const {
    return !(rhs == *this);
  }
};
