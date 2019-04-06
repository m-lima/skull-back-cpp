#pragma once

#include <string>

struct QuickValue {
private:
  std::string mType;
  std::string mAmount;
  std::string mIcon;

public:
  template <typename A, typename B, typename C>
  QuickValue(A && type,
             B && amount,
             C && icon)
      : mType{std::forward<A>(type)},
        mAmount{std::forward<B>(amount)},
        mIcon{std::forward<C>(icon)} {}

  inline std::string json() const {
    return R"({"type":")" + mType
           + R"(","amount":)" + mAmount
           + R"(,"icon":")" + mIcon
           + "\"}";
  }

  inline const std::string & type() const {
    return mType;
  }

  inline const std::string & amount() const {
    return mAmount;
  }

  inline const std::string & icon() const {
    return mIcon;
  }

  inline bool operator==(const QuickValue & rhs) const {
    return mType == rhs.mType && mAmount == rhs.mAmount && mIcon == rhs.mIcon;
  }

  inline bool operator!=(const QuickValue & rhs) const {
    return !(rhs == *this);
  }

  friend std::ostream & operator<<(std::ostream & stream,
                                   const QuickValue & value) {
    stream << R"({"type":")" << value.mType
           << R"(","amount":)" << value.mAmount
           << R"(,"icon":)" << value.mIcon
           << '}';
    return stream;
  }
};
