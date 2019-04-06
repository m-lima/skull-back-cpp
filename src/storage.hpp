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
//  using SkullValues = std::unordered_map<User, std::vector<SkullValue>>;
  std::unordered_map<User, std::vector<QuickValue>> mQuickValues;
  std::unordered_map<User, std::vector<SkullValue>> mSkullValues;
  std::unordered_map<User, std::mutex> mMutexes;

public:
  inline bool authorized(const User & user) const {
    return user != constant::user::UNKNOWN && mQuickValues.find(user) != mQuickValues.end();
  }

//  std::optional<std::vector<QuickValue>> getQuickValues(const User & user) const;
//  std::unique_ptr<std::vector<QuickValue>> getQuickValues(const User & user) const;
  const std::vector<QuickValue> * getQuickValues(const User & user) const;
  const std::vector<SkullValue> * getSkullValues(const User & user);
  bool addSkullValue(const User & user, SkullValue && skullValue);
  bool deleteSkullValue(const User & user, const SkullValue & skullValue);
//  void commitSkullChanges(const SkullValues::const_iterator & skulls,
//                          std::unique_lock<std::mutex> && lock);
};



