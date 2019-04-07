#pragma once

#include <mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "constants.hpp"
#include "quick_value.hpp"
#include "response.hpp"
#include "skull_value.hpp"
#include "user.hpp"

class Storage {
private:
  template <typename T>
  struct LockedVector {
    std::mutex mutex;
    std::vector<T> vector;

    LockedVector(std::vector<T> && vector) : vector{std::move(vector)} {}
    LockedVector(const std::vector<T> &) = delete;

    LockedVector(LockedVector && vector) = default;
    LockedVector(const LockedVector &) = delete;
    LockedVector & operator=(const LockedVector &) = delete;
  };

  std::unordered_map<User, LockedVector<QuickValue>> mQuickValues;
  std::unordered_map<User, LockedVector<SkullValue>> mSkullValues;

  template <typename T>
  struct TypeMap {
  };

  template <>
  struct TypeMap<QuickValue> {
    static constexpr auto Storage::* const map = &Storage::mQuickValues;
  };

  template <>
  struct TypeMap<SkullValue> {
    static constexpr auto Storage::* const map = &Storage::mSkullValues;
  };

  template <typename T, typename S>
  void stream(const std::vector<T> & vector, S & stream) const {
    stream << '[';
    auto end = vector.cend() - 1;
    for (auto it = vector.cbegin(); it != end; ++it) {
      stream << it->json() << ',';
    }
    stream << end->json() << ']';
  }

  template <typename T>
  void save(const User &, LockedVector<T> &&) const {
  }

public:
  Storage();

  inline bool authorized(const User & user) const {
    return user != constant::user::UNKNOWN && mQuickValues.find(user) != mQuickValues.cend();
  }

  template <typename T>
  std::string get(const User & user) {
    const auto values = (this->*TypeMap<T>::map).find(user);
    if (values == (this->*TypeMap<T>::map).cend()) return "[]";
    if (values->second.vector.empty()) return "[]";

    std::lock_guard lock{values->second.mutex};

    std::stringstream output;
    stream(values->second.vector, output);
    return output.str();
  }

  template <typename T, typename S>
  void stream(const User & user, S & output) {
    const auto values = (this->*TypeMap<T>::map).find(user);
    if (values == (this->*TypeMap<T>::map).cend() || values->second.vector.empty()) {
      output << "[]";
      return;
    }

    std::lock_guard lock{values->second.mutex};

    stream(values->second.vector, output);
  }

  template <typename T>
  bool add(const User & user, T && value) {
    const auto values = (this->*TypeMap<T>::map).find(user);
    if (values == (this->*TypeMap<T>::map).cend()) return false;

    std::unique_lock lock{values->second.mutex};
    values->second.vector.push_back(std::forward<T>(value));

//    std::thread saver{&save, user.name, std::move(values->second)};
//    saver.detach();
    return true;
  }

  template <typename T>
  bool remove(const User & user, T && value) {
    const auto values = (this->*TypeMap<T>::map).find(user);
    if (values == (this->*TypeMap<T>::map).cend()) return false;

    std::unique_lock lock{values->second.mutex};

    auto entry = std::find(values->second.vector.begin(), values->second.vector.end(), value);
    if (entry == values->second.vector.end()) return false;
    values->second.vector.erase(entry);

//    std::thread saver{&save, user.name, std::move(values->second)};
//    saver.detach();
    return true;
  }

  template <typename T>
  inline std::size_t estimateSize(const User & user) const {
    const auto values = (this->*TypeMap<T>::map).find(user);
    if (values == (this->*TypeMap<T>::map).cend()) return 0;

    return values->second.vector.size() * 50;
  }
};



