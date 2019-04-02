#pragma once

#include <string>

class SkullValue {
private:
  std::string type;
  std::string amount;
  std::string millis;

public:
  template<typename A, typename B, typename C>
  SkullValue(A && type,
             B && amount,
             C && millis)
      : type{std::forward<A>(type)},
        amount{std::forward<B>(amount)},
        millis{std::forward<C>(millis)} {}

  inline const std::string & getType() const {
    return type;
  }

  inline const std::string & getAmount() const {
    return amount;
  }

  inline const std::string & getmillis() const {
    return millis;
  }

//  template <typename T>
//  SkullValue(T && type,
//             const double amount,
//             const long millis)
//      : type{std::forward<T>(type)},
//        amount{std::to_string(amount)},
//        millis{std::to_string(millis)} {}

//  SkullValue(const SkullValue & other) : type{other.type}, amount{other.amount}, millis{other.millis} {}
//  SkullValue(SkullValue && other)
//      : type{std::move(other.type)},
//        amount{std::move(other.amount)},
//        millis{std::move(other.millis)} {}
//
//  SkullValue & operator=(const SkullValue & other) {
//    if (&other == this) {
//      return *this;
//    }
//
//    SkullValue fresh{other.type, other.amount, other.millis};
//
//    return fresh;
//  }

  inline std::string json() const {
    return R"({"type":")" + type + R"(","amount":)" + amount + R"(,"millis":)" + millis + "}";
  }

  inline bool operator==(const SkullValue & rhs) const {
    return type == rhs.type && amount == rhs.amount && millis == rhs.millis;
  }

  inline bool operator!=(const SkullValue & rhs) const {
    return !(rhs == *this);
  }
};
