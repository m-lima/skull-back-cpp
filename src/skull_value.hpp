#pragma once

#include <iomanip>

#include <string>

class SkullValue {
private:
  std::string mType;
  std::string mAmount;
  std::string mMillis;

public:
  static constexpr const auto size = 3;

  SkullValue(const SkullValue &) = delete;
  SkullValue(SkullValue &&) = default;
  SkullValue & operator=(const SkullValue &) = delete;
  SkullValue & operator=(SkullValue &&) = default;

  template <typename A, typename B, typename C>
  SkullValue(A && type,
             B && amount,
             C && millis)
      : mType{std::forward<A>(type)},
        mAmount{std::forward<B>(amount)},
        mMillis{std::forward<C>(millis)} {}

  [[nodiscard]]
  inline const std::string & type() const {
    return mType;
  }

  [[nodiscard]]
  inline const std::string & amount() const {
    return mAmount;
  }

  [[nodiscard]]
  inline const std::string & millis() const {
    return mMillis;
  }

  inline bool operator==(const SkullValue & rhs) const {
    return mType == rhs.mType && mAmount == rhs.mAmount && mMillis == rhs.mMillis;
  }

  inline bool operator!=(const SkullValue & rhs) const {
    return !(rhs == *this);
  }

  template <typename T>
  inline T & json(T & stream) const {
    stream << R"({"type":")" << mType
           << R"(","amount":)" << mAmount
           << R"(,"millis":)" << mMillis
           << '}';
    return stream;
  }

  template <typename T>
  inline T & tsv(T & stream) const {
    stream << mType << '\t' << mAmount << '\t' << mMillis;
    return stream;
  }
};
