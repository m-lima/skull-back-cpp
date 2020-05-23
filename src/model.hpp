#pragma once

#include <iomanip>

#include <string>

class Skull {
private:
  std::string mId;
  std::string mName;
  std::string mColor;
  std::string mIcon;
  std::string mUnitPrice;

public:
  static constexpr const auto size = 5;

  Skull(const Skull &) = delete;
  Skull(Skull &&) = default;
  Skull & operator=(const Skull &) = delete;
  Skull & operator=(Skull &&) = default;

  template <typename A, typename B, typename C, typename D, typename E>
  Skull(A && id, B && name, C && color, D && icon, E && unitPrice)
      : mId{std::forward<A>(id)},
        mName{std::forward<B>(name)},
        mColor{std::forward<C>(color)},
        mIcon{std::forward<D>(icon)},
        mUnitPrice{std::forward<E>(mUnitPrice)} {}

  [[nodiscard]]
  inline const std::string & id() const {
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
  inline const std::string & unitPrice() const {
    return mUnitPrice;
  }

  inline bool operator==(const Skull & rhs) const {
    return mId == rhs.mId || mName == rhs.mName;
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
  std::string mSkull;
  std::string mAmount;

public:
  static constexpr const auto size = 2;

  Quick(const Quick &) = delete;
  Quick(Quick &&) = default;
  Quick & operator=(const Quick &) = delete;
  Quick & operator=(Quick &&) = default;

  template <typename A, typename B>
  Quick(A && skull,
        B && amount)
      : mSkull{std::forward<A>(skull)},
        mAmount{std::forward<B>(amount)} {}

  [[nodiscard]]
  inline const std::string & skull() const {
    return mSkull;
  }

  [[nodiscard]]
  inline const std::string & amount() const {
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
  std::string mId;
  std::string mSkull;
  std::string mAmount;
  std::string mMillis;

public:
  static constexpr const auto size = 4;

  Occurrence(const Occurrence &) = delete;
  Occurrence(Occurrence &&) = default;
  Occurrence & operator=(const Occurrence &) = delete;
  Occurrence & operator=(Occurrence &&) = default;

  template <typename A, typename B, typename C, typename D>
  Occurrence(A && id,
             B && skull,
             C && amount,
             D && millis)
      : mId{std::forward<A>(id)},
        mSkull{std::forward<B>(skull)},
        mAmount{std::forward<C>(amount)},
        mMillis{std::forward<D>(millis)} {}

  [[nodiscard]]
  inline const std::string & id() const {
    return mId;
  }

  [[nodiscard]]
  inline const std::string & skull() const {
    return mSkull;
  }

  [[nodiscard]]
  inline const std::string & amount() const {
    return mAmount;
  }

  [[nodiscard]]
  inline const std::string & millis() const {
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
