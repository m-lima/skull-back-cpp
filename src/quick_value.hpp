#pragma once

#include <string>

struct QuickValue {
private:
  std::string mType;
  std::string mAmount;
  std::string mIcon;

public:
  static constexpr const auto size = 3;

  QuickValue(const QuickValue &) = delete;
  QuickValue(QuickValue &&) = default;
  QuickValue & operator=(const QuickValue &) = delete;
  QuickValue & operator=(QuickValue &&) = default;

  template <typename A, typename B, typename C>
  QuickValue(A && type,
             B && amount,
             C && icon)
      : mType{std::forward<A>(type)},
        mAmount{std::forward<B>(amount)},
        mIcon{std::forward<C>(icon)} {}

  [[nodiscard]]
  inline const std::string & type() const {
    return mType;
  }

  [[nodiscard]]
  inline const std::string & amount() const {
    return mAmount;
  }

  [[nodiscard]]
  inline const std::string & icon() const {
    return mIcon;
  }

  inline bool operator==(const QuickValue & rhs) const {
    return mType == rhs.mType && mAmount == rhs.mAmount && mIcon == rhs.mIcon;
  }

  inline bool operator!=(const QuickValue & rhs) const {
    return !(rhs == *this);
  }

  template <typename T>
  inline T & json(T & stream) const {
    stream << R"({"type":")" << mType
           << R"(","amount":)" << mAmount
           << R"(,"icon":")" << mIcon
           << "\"}";
    return stream;
  }

  template <typename T>
  inline T & tsv(T & stream) const {
    stream << mType << '\t' << mAmount << '\t' << mIcon;
    return stream;
  }
};
