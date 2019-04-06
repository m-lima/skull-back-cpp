#include "storage.hpp"

#include <regex>
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

    const auto userPath = (boost::filesystem::path{constant::file::ROOT}
                           / skullValues->first.name
                           / constant::file::SKULL).generic_string();

    std::ofstream file;
    file.open(userPath);

    if (!file.good()) {
      spdlog::error("Failed to open {:s} for saving", userPath);
      return;
    }

    convertToString(skullValues->second, file);

    file.close();
  }

  template <typename T>
  std::optional<std::smatch> parseValue(std::ifstream & file) {
    std::string buffer;

    std::getline(file, buffer, '{');
    if (buffer.empty() || buffer[0] == ']') {
      return {};
    }

    std::getline(file, buffer, '}');

    std::smatch match;
    static_assert(std::disjunction_v<std::is_same<T, QuickValue>, std::is_same<T, SkullValue>>);
    if constexpr (std::is_same<T, QuickValue>::value) {
      static const std::regex quickValueRegex{R""("type":"(.+)","amount":([0-9]+),"icon":"(.+)")""};
      if (!std::regex_match(buffer, match, quickValueRegex)) {
        spdlog::error("Unmatched value entry {:s}", buffer);
        return {};
      }
    } else if (std::is_same<T, SkullValue>::value) {
      static const std::regex skullValueRegex{R""("type":"(.+)","amount":([0-9]+),"millis":([0-9]+))""};
      if (!std::regex_match(buffer, match, skullValueRegex)) {
        spdlog::error("Unmatched value entry {:s}", buffer);
        return {};
      }
    }

    if (match.size() != 4) {
      spdlog::error("Malformed value entry {:s}", buffer);
      return {};
    }

    return std::make_optional(match);
  }

  template <typename T>
  std::vector<T> loadValues(const std::string & path) {
    std::ifstream file;
    file.open(path);

    if (!file.good()) {
      spdlog::error("Failed to open {:s} for loading", path);
      return {};
    }

    std::vector<T> output;
    while (true) {
      auto match = parseValue<T>(file);
      if (!match) break;
      output.emplace_back((*match)[1], (*match)[2], (*match)[3]);
    }

    return output;
  }
}

Storage::Storage() {
  auto root = boost::filesystem::path{constant::file::ROOT};

  for (auto it{boost::filesystem::directory_iterator{root}}; it != boost::filesystem::directory_iterator{}; ++it) {
    auto user = it->path().filename().generic_string();
    spdlog::info("Found user: {:s}", user);
    mQuickValues.try_emplace(user, loadValues<QuickValue>((it->path() / constant::file::QUICK).generic_string()));
    mSkullValues.try_emplace(user, loadValues<SkullValue>((it->path() / constant::file::SKULL).generic_string()));
    mMutexes.try_emplace(user);
  }
}

std::string Storage::getQuickValues(const User & user) const {
  auto quickValues = mQuickValues.find(user);
  if (quickValues == mQuickValues.end()) return "[]";

  std::stringstream stream;
  convertToString(quickValues->second, stream);
  return {stream.str()};
}

std::string Storage::getSkullValues(const User & user) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return "[]";

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return "[]";

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
