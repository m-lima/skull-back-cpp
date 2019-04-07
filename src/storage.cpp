#include "storage.hpp"

#include <regex>
#include <sstream>
#include <thread>

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"

namespace {
  template <typename T>
  struct TypeProps {
  };

  template <>
  struct TypeProps<QuickValue> {
    static constexpr const auto & path = constant::file::QUICK;

    static const auto & regex() {
      static const std::regex regex{R""("type":"(.+)","amount":([0-9]+),"icon":"(.+)")""};
      return regex;
    }
  };

  template <>
  struct TypeProps<SkullValue> {
    static constexpr const auto & path = constant::file::SKULL;

    static const auto & regex() {
      static const std::regex regex{R""("type":"(.+)","amount":([0-9]+),"millis":([0-9]+))""};
      return regex;
    }
  };

  template <typename T, typename S>
  inline void convertToString(const std::vector<T> & vector, S & stream) {
    if (vector.empty()) {
      stream << "[]";
      return;
    }

    stream << '[';
    auto end = vector.cend() - 1;
    for (auto it = vector.cbegin(); it != end; ++it) {
      stream << *it << ',';
    }
    stream << *end << ']';
  }

  template <typename T>
  void save(const std::string & userName,
            const typename std::unordered_map<User, std::vector<T>>::const_iterator && values,
            std::unique_lock<std::mutex> &&) {

    const auto userPath = (boost::filesystem::path{constant::file::ROOT}
                           / userName
                           / TypeProps<T>::path).generic_string();

    std::ofstream file;
    file.open(userPath);

    if (!file.good()) {
      spdlog::error("Failed to open {:s} for saving", userPath);
      return;
    }

    convertToString(values->second, file);

    spdlog::info("Updated {:s}", userPath);
    file.close();
  }

  template <typename T>
  std::optional<std::smatch> parseValue(std::ifstream & file) {
    static_assert(std::disjunction_v<std::is_same<T, QuickValue>, std::is_same<T, SkullValue>>);
    std::string buffer;

    std::getline(file, buffer, '{');
    if (buffer.empty() || *(buffer.cend() - 1) == ']') {
      return {};
    }

    std::getline(file, buffer, '}');

    std::smatch match;
    if (!std::regex_match(buffer, match, TypeProps<T>::regex())) {
      spdlog::error("Unmatched value entry {:s}", buffer);
      return {};
    }

    if (match.size() != 4) {
      spdlog::error("Malformed value entry {:s}", buffer);
      return {};
    }

    return std::make_optional(match);
  }

  template <typename T>
  std::vector<T> load(const boost::filesystem::directory_iterator & it) {
    static_assert(std::disjunction_v<std::is_same<T, QuickValue>, std::is_same<T, SkullValue>>);

    const auto path = (it->path() / TypeProps<T>::path).generic_string();
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
    mQuickValues.try_emplace(user, load<QuickValue>(it));
    mSkullValues.try_emplace(user, load<SkullValue>(it));
    mMutexes.try_emplace(user);
  }
}

template <>
std::string Storage::get<QuickValue>(const User & user) {
  const auto quickValues = mQuickValues.find(user);
  if (quickValues == mQuickValues.cend()) return "[]";

  std::stringstream stream;
  convertToString(quickValues->second, stream);
  return {stream.str()};
}

template <>
std::string Storage::get<SkullValue>(const User & user) {
  const auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.cend()) return "[]";

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return "[]";

  std::lock_guard lock{mutex->second};

  std::stringstream stream;
  convertToString(skullValues->second, stream);
  return {stream.str()};
}

template <>
bool Storage::add<SkullValue>(const User & user, SkullValue && value) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return false;

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return false;

  std::unique_lock lock{mutex->second};
  skullValues->second.push_back(std::move(value));

  std::thread saver{save<SkullValue>, user.name, std::move(skullValues), std::move(lock)};
  saver.detach();
  return true;
}

template <>
bool Storage::remove<SkullValue>(const User & user, SkullValue && value) {
  auto skullValues = mSkullValues.find(user);
  if (skullValues == mSkullValues.end()) return false;

  auto mutex = mMutexes.find(user);
  if (mutex == mMutexes.end()) return false;

  std::unique_lock lock{mutex->second};

  auto entry = std::find(skullValues->second.begin(), skullValues->second.end(), value);
  if (entry == skullValues->second.end()) return false;

  skullValues->second.erase(entry);
  std::thread saver{save<SkullValue>, user.name, std::move(skullValues), std::move(lock)};
  saver.detach();
  return true;
}
