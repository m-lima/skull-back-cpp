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

public:
  Storage();

  inline bool authorized(const User & user) const {
    return user != constant::user::UNKNOWN && mQuickValues.find(user) != mQuickValues.end();
  }

  std::string getQuickValues(const User & user) const;
  std::string getSkullValues(const User & user);
  bool addSkullValue(const User & user, SkullValue && skullValue);
  bool deleteSkullValue(const User & user, const SkullValue & skullValue);

  template <typename S>
  void streamSkullValues(const User & user, S & stream) const {
    auto values = mSkullValues.find(user);
    if (values == mSkullValues.end()) return;

    if (values->second.empty()) {
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



