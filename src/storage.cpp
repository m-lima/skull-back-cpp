#include "storage.hpp"

#include "constants.hpp"

std::optional<const std::vector<QuickValue>> Storage::getQuickValuePath(const User & user) const {
  auto quickValues = mQuickValues.find(user);

  if (quickValues == mQuickValues.end()) {
    return {};
  }

  return std::make_optional(quickValues->second);
}

std::optional<const std::vector<SkullValue>> Storage::getSkullValue(const User & user) {
  auto skullValues = mSkullValues.find(user);

  if (skullValues == mSkullValues.end()) {
    return {};
  }

  auto mutex = mMutexes.find(user);

  if (mutex == mMutexes.end()) {
    return {};
  }

  std::lock_guard lock{mutex->second};
  return std::make_optional(skullValues->second);
}

bool Storage::addSkullValue(const User & user, const SkullValue & skullValue) {
  auto skullValues = mSkullValues.find(user);

  if (skullValues == mSkullValues.end()) {
    return false;
  }

  auto mutex = mMutexes.find(user);

  if (mutex == mMutexes.end()) {
    return false;
  }

  std::lock_guard lock{mutex->second};
  skullValues->second.push_back(skullValue);
  commitSkullChanges(user);
  return true;
}

bool Storage::deleteSkullValue(const User & user, const SkullValue & skullValue) {
  auto skullValues = mSkullValues.find(user);

  if (skullValues == mSkullValues.end()) {
    return false;
  }

  auto mutex = mMutexes.find(user);

  if (mutex == mMutexes.end()) {
    return false;
  }

  std::lock_guard lock{mutex->second};

  auto entry = std::find(skullValues->second.begin(), skullValues->second.end(), skullValue);

  if (entry == skullValues->second.end()) {
    return false;
  }

  skullValues->second.erase(entry);
  commitSkullChanges(user);
  return true;
}

void Storage::commitSkullChanges(const User & user) {

}
