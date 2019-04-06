#pragma once

#include <string>
#include <ostream>

#include "constants.hpp"

class SkullValue {
private:
  std::string mType;
  std::string mAmount;
  std::string mMillis;

public:
  template <typename A, typename B, typename C>
  SkullValue(A && type,
             B && amount,
             C && millis)
      : mType{std::forward<A>(type)},
        mAmount{std::forward<B>(amount)},
        mMillis{std::forward<C>(millis)} {}

//  SkullValue(const SkullValue &) = delete;

  inline const std::string & type() const {
    return mType;
  }

  inline const std::string & amount() const {
    return mAmount;
  }

  inline const std::string & millis() const {
    return mMillis;
  }

  inline bool valid() const {
    return mType != constant::query::UNDEFINED
           && mAmount != constant::query::UNDEFINED
           && mMillis != constant::query::UNDEFINED;
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
    return R"({"type":")" + mType + R"(","amount":)" + mAmount + R"(,"millis":)" + mMillis + "}";
  }

  inline bool operator==(const SkullValue & rhs) const {
    return mType == rhs.mType && mAmount == rhs.mAmount && mMillis == rhs.mMillis;
  }

  inline bool operator!=(const SkullValue & rhs) const {
    return !(rhs == *this);
  }

  friend std::ostream & operator<<(std::ostream & stream,
                                   const SkullValue & value) {
    stream << R"({"type":")" << value.mType
           << R"(","amount":)" << value.mAmount
           << R"(,"millis":)" << value.mMillis
           << '}';
    return stream;
  }
};
