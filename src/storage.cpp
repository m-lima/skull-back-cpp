#include "storage.hpp"

#include <sstream>
#include <thread>

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"

namespace {
  template <typename T, typename S>
  void convertToString(const std::vector<T> & vector, S & stream) {
    stream << '[';
    auto end = vector.cend() - 1;
    for (auto it = vector.cbegin(); it != end; ++it) {
      stream << *it << ',';
    }
    stream << *end << ']';
  }

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

    convertToString(skullValues->second, file);

    file.close();
  }
}

Storage::Storage() {
  mQuickValues.try_emplace("user");
  mSkullValues.try_emplace("user");
  mMutexes.try_emplace("user");

  mQuickValues["user"].emplace_back("type", "1", "icon");
  mQuickValues["user"].emplace_back("type2", "2", "icon");
  mQuickValues["user"].emplace_back("type3", "3", "icon");

  mSkullValues["user"].emplace_back("type", "1", "icon");
  mSkullValues["user"].emplace_back("type2", "2", "icon");
  mSkullValues["user"].emplace_back("type3", "3", "icon");
}

std::optional<std::string> Storage::getQuickValues(const User & user) const {
  auto quickValues = mQuickValues.find(user);
  if (quickValues == mQuickValues.end()) return {};

  std::stringstream stream;
  convertToString(quickValues->second, stream);
  return {stream.str()};
}

std::optional<std::string> Storage::getSkullValues(const User & user) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return {};

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return {};

  std::lock_guard lock{mutex->second};

  std::stringstream stream;
  convertToString(skullValues->second, stream);
  return {stream.str()};
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

