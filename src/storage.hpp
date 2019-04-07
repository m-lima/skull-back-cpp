#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

#include "constants.hpp"
#include "quick_value.hpp"
#include "response.hpp"
#include "skull_value.hpp"
#include "user.hpp"

class Storage {
private:
  std::unordered_map<User, std::vector<QuickValue>> mQuickValues;
  std::unordered_map<User, std::vector<SkullValue>> mSkullValues;
  std::unordered_map<User, std::mutex> mMutexes;

  template <typename T>
  struct TypeMap {
  };

  template <>
  struct TypeMap<QuickValue> {
    static constexpr const auto Storage::* const map = &Storage::mQuickValues;
  };

  template <>
  struct TypeMap<SkullValue> {
    static constexpr const auto Storage::* const map = &Storage::mSkullValues;
  };

public:
  Storage();

  template <typename T>
  std::string get(const User & user);
  template <typename T>
  bool add(const User & user, T && value);
  template <typename T>
  bool remove(const User & user, T && value);

  inline bool authorized(const User & user) const {
    return user != constant::user::UNKNOWN && mQuickValues.find(user) != mQuickValues.cend();
  }

  template <typename T>
  inline std::size_t estimateSize(const User & user) const {
    const auto values = (this->*TypeMap<T>::map).find(user);
    if (values == (this->*TypeMap<T>::map).cend()) return 0;

    return values->second.size() * 50;
  }

  template <typename T, typename S>
  inline void streamValues(const User & user, S & stream) const {
    const auto values = (this->*TypeMap<T>::map).find(user);

    if (values == (this->*TypeMap<T>::map).cend() || values->second.empty()) {
      stream << "[]";
      return;
    }

    stream << "[";
    auto end = values->second.cend() - 1;
    for (auto it = values->second.cbegin(); it != end; ++it) {
      stream << it->json() << ",";
    }
    stream << end->json() << "]";
  }
};



