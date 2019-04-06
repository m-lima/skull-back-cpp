#include "storage.hpp"

#include <thread>

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"

namespace {
  void save(const std::unordered_map<User, std::vector<SkullValue>>::const_iterator & skullValues,
            std::unique_lock<std::mutex> &&) {

    auto userPath = (boost::filesystem::path{constant::file::ROOT}
                     / skullValues->first.name
                     / constant::file::SKULL).generic_string();

    std::ofstream file;
    file.open(userPath);

    if (!file.good()) {
      spdlog::error("Failed to open {:s}", userPath);
      return;
    }

    file << '[';
    auto end = skullValues->second.cend() - 1;
    for (auto it = skullValues->second.cbegin(); it != end; ++it) {
      file << *it << ',';
    }
    file << *end << ']';

    file.close();
  }
}

const std::vector<QuickValue> * Storage::getQuickValues(const User & user) const {
  auto quickValues = mQuickValues.find(user);
  if (quickValues == mQuickValues.end()) return nullptr;

  return &quickValues->second;
}

const std::vector<SkullValue> * Storage::getSkullValues(const User & user) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return nullptr;

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return nullptr;

  mutex->second.lock();
  std::lock_guard lock{mutex->second};
  return &skullValues->second;
}

bool Storage::addSkullValue(const User & user, SkullValue && skullValue) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return false;

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return false;

  std::unique_lock lock{mutex->second};
  skullValues->second.push_back(std::move(skullValue));

  std::thread saver{save, skullValues, std::move(lock)};
  saver.detach();
  return true;
}

bool Storage::deleteSkullValue(const User & user,
                               const SkullValue & skullValue) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return false;

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return false;

  std::unique_lock lock{mutex->second};

  auto entry = std::find(skullValues->second.begin(), skullValues->second.end(), skullValue);
  if (entry == skullValues->second.end()) return false;

  skullValues->second.erase(entry);
  std::thread saver{save, skullValues, std::move(lock)};
  saver.detach();
  return true;
}

