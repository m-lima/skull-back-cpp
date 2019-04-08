#pragma once

#include <string>

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

  inline const std::string & type() const {
    return mType;
  }

  inline const std::string & amount() const {
    return mAmount;
  }

  inline const std::string & millis() const {
    return mMillis;
  }

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
