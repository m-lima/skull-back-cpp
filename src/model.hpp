#pragma once

#include <iomanip>
#include <limits>
#include <string>

namespace {
  template<typename T>
  T stot(std::string_view string);

  template<>
  unsigned short stot<unsigned short>(std::string_view string) {
    std::string buffer{string};
    auto integer = std::stoi(buffer);
    if (integer < 0) {
      throw std::runtime_error{"underflow"};
    }
    if (integer > std::numeric_limits<unsigned short>::max()) {
      throw std::runtime_error{"overflow"};
    }
    return static_cast<unsigned short>(integer);
  }

  template<>
  long stot<long>(std::string_view string) {
    std::string buffer{string};
    return std::stol(buffer);
  }

  template<>
  float stot<float>(std::string_view string) {
    std::string buffer{string};
    return std::stof(buffer);
  }
}

class Skull {
private:
  unsigned short mId;
  std::string mName;
  std::string mColor;
  std::string mIcon;
  float mUnitPrice;

public:
  static constexpr const auto size = 5;

  Skull(const Skull &) = delete;
  Skull(Skull &&) = default;
  Skull & operator=(const Skull &) = delete;
  Skull & operator=(Skull &&) = default;

  template <typename A, typename B, typename C>
  Skull(unsigned short id, A && name, B && color, C && icon, float unitPrice)
      : mId{id},
        mName{std::forward<A>(name)},
        mColor{std::forward<B>(color)},
        mIcon{std::forward<C>(icon)},
        mUnitPrice{unitPrice} {}

  Skull(std::array<std::string_view, size> params)
      : mId{stot<unsigned short>(params[0])},
        mName{params[1]},
        mColor{params[2]},
        mIcon{params[3]},
        mUnitPrice{stot<float>(params[4])} {}

  [[nodiscard]]
  inline const unsigned short & id() const {
    return mId;
  }

  [[nodiscard]]
  inline const std::string & name() const {
    return mName;
  }

  [[nodiscard]]
  inline const std::string & color() const {
    return mColor;
  }

  [[nodiscard]]
  inline const std::string & icon() const {
    return mIcon;
  }

  [[nodiscard]]
  inline const float & unitPrice() const {
    return mUnitPrice;
  }

  inline bool operator==(const Skull & rhs) const {
    return mId == rhs.mId;
  }

  inline bool operator!=(const Skull & rhs) const {
    return !(rhs == *this);
  }

  template <typename T>
  inline T & json(T & stream) const {
    stream << R"({"id":)" << mId
           << R"(,"name":")" << mName << '"'
           << R"(,"color":")" << mColor << '"'
           << R"(,"icon":")" << mIcon << '"'
           << R"(,"unitPrice":)" << mUnitPrice
           << '}';
    return stream;
  }

  template <typename T>
  inline T & tsv(T & stream) const {
    stream << mId << '\t'
           << mName << '\t'
           << mColor << '\t'
           << mIcon << '\t'
           << mUnitPrice;
    return stream;
  }
};

struct Quick {
private:
  unsigned short mSkull;
  float mAmount;

public:
  static constexpr const auto size = 2;

  Quick(const Quick &) = delete;
  Quick(Quick &&) = default;
  Quick & operator=(const Quick &) = delete;
  Quick & operator=(Quick &&) = default;

  Quick(unsigned short skull, float amount)
      : mSkull{skull},
        mAmount{amount} {}

  Quick(std::array<std::string_view, size> params)
      : mSkull{stot<unsigned short>(params[0])},
        mAmount{stot<float>(params[1])} {}

  [[nodiscard]]
  inline const unsigned short & skull() const {
    return mSkull;
  }

  [[nodiscard]]
  inline const float & amount() const {
    return mAmount;
  }

  inline bool operator==(const Quick & rhs) const {
    return mSkull == rhs.mSkull && mAmount == rhs.mAmount;
  }

  inline bool operator!=(const Quick & rhs) const {
    return !(rhs == *this);
  }

  template <typename T>
  inline T & json(T & stream) const {
    stream << R"({"skull":)" << mSkull
           << R"(,"amount":)" << mAmount
           << '}';
    return stream;
  }

  template <typename T>
  inline T & tsv(T & stream) const {
    stream << mSkull << '\t' << mAmount;
    return stream;
  }
};

struct Occurrence {
private:
  unsigned short mId;
  unsigned short mSkull;
  float mAmount;
  long mMillis;

public:
  static constexpr const auto size = 4;

  Occurrence(const Occurrence &) = delete;
  Occurrence(Occurrence &&) = default;
  Occurrence & operator=(const Occurrence &) = delete;
  Occurrence & operator=(Occurrence &&) = default;

  Occurrence(unsigned short id,
             unsigned short skull,
             float amount,
             long millis)
      : mId{id},
        mSkull{skull},
        mAmount{amount},
        mMillis{millis} {}

  Occurrence(std::array<std::string_view, size> params)
      : mId{stot<unsigned short>(params[0])},
        mSkull{stot<unsigned short>(params[1])},
        mAmount{stot<float>(params[2])},
        mMillis{stot<long>(params[3])} {}

  [[nodiscard]]
  inline const unsigned short & id() const {
    return mId;
  }

  [[nodiscard]]
  inline const unsigned short & skull() const {
    return mSkull;
  }

  [[nodiscard]]
  inline const float & amount() const {
    return mAmount;
  }

  [[nodiscard]]
  inline const long & millis() const {
    return mMillis;
  }

  inline bool operator==(const Occurrence & rhs) const {
    return mId == rhs.mId;
  }

  inline bool operator!=(const Occurrence & rhs) const {
    return !(rhs == *this);
  }

  template <typename T>
  inline T & json(T & stream) const {
    stream << R"({"id":)" << mId
           << R"(,"skull":)" << mSkull
           << R"(,"amount":)" << mAmount
           << R"(,"millis":)" << mMillis
           << '}';
    return stream;
  }

  template <typename T>
  inline T & tsv(T & stream) const {
    stream << mId << '\t'
           << mSkull << '\t'
           << mAmount << '\t'
           << mMillis;
    return stream;
  }
};
