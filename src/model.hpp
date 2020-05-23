#pragma once

#include <iomanip>
#include <string>

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

  template <typename A>
  Quick(A && skull, float amount)
      : mSkull{std::forward<A>(skull)},
        mAmount{amount} {}

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
           << R"(","amount":)" << mAmount
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
