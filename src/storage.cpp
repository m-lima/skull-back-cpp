#include "storage.hpp"

#include <regex>

#include <spdlog/spdlog.h>

namespace {
  template <typename T>
  struct TypeProps {
  };

  template <>
  struct TypeProps<QuickValue> {
    static const auto & regex() {
      static const std::regex regex{R""("type":"(.+)","amount":([0-9]+),"icon":"(.+)")""};
      return regex;
    }
  };

  template <>
  struct TypeProps<SkullValue> {
    static const auto & regex() {
      static const std::regex regex{R""("type":"(.+)","amount":([0-9]+),"millis":([0-9]+))""};
      return regex;
    }
  };
}

Storage::Storage() {
  UserIterator::forEach([this](const User & user) {
    spdlog::info("Found user: {:s}", user.name);
    mQuickValues.try_emplace(user, load<QuickValue>(user));
    mSkullValues.try_emplace(user, load<SkullValue>(user));
  });
}

template <typename T>
std::optional<T> Storage::parseValue(std::ifstream & file) {
  std::string buffer;

  std::getline(file, buffer, '{');
  if (buffer.empty() || *(buffer.cend() - 1) == ']') {
    return {};
  }

  std::getline(file, buffer, '}');

  std::smatch match;
  if (!std::regex_match(buffer, match, ::TypeProps<T>::regex())) {
    spdlog::error("Unmatched value entry {:s}", buffer);
    return {};
  }

  if (match.size() != 4) {
    spdlog::error("Malformed value entry {:s}", buffer);
    return {};
  }

  return std::make_optional<T>(match[1], match[2], match[3]);
}
