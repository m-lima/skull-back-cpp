#pragma once

#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

#include "constants.hpp"
#include "quick_value.hpp"
#include "skull_value.hpp"
#include "user.hpp"

class Storage {
private:
  std::unordered_map<User, std::vector<QuickValue>> mQuickValues;
  std::unordered_map<User, std::vector<SkullValue>> mSkullValues;
  std::unordered_map<User, std::mutex> mMutexes;

public:
  inline bool authorized(const User & user) const {
    return user != constant::user::UNKNOWN && mQuickValues.find(user) != mQuickValues.end();
  }

  std::optional<const std::vector<QuickValue>> getQuickValuePath(const User & user) const;
  std::optional<const std::vector<SkullValue>> getSkullValue(const User & user);
  bool addSkullValue(const User & user, const SkullValue & skullValue);
  bool deleteSkullValue(const User & user, const SkullValue & skullValue);
  void commitSkullChanges(const User & user);
};



