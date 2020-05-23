#include "storage.hpp"

#include <array>

#include <spdlog/spdlog.h>

namespace {
  template <typename T>
  std::optional<T> parseValue(std::string_view view) {
    std::array<std::string_view, T::size> segments;

    std::size_t index{0};
    for (auto i = 0; i < T::size - 1; ++i) {
      auto next = view.find('\t', index);
      if (next == std::string_view::npos) return {};

      segments[i] = view.substr(index, next - index);
      index = next + 1;
    }

    if (view.find('\t', index) != std::string_view::npos) return {};

    segments[T::size - 1] = view.substr(index);

    try {
      return std::make_optional(segments);
    } catch(const std::exception &) {
      return {};
    }
  }
}

Storage::Storage() {
  UserIterator::forEach([this](const User & user) {
    auto skull = mSkulls.try_emplace(user, std::vector<Skull>{});
    auto quick = mQuicks.try_emplace(user, std::vector<Quick>{});
    auto occurrence = mOccurrences.try_emplace(user, std::vector<Occurrence>{});

    if (skull.second) {
      load(user, skull.first->second.vector);
    } else {
      spdlog::warn("Failed to populate skull map for {:s}", user.name);
    }

    if (quick.second) {
      load(user, quick.first->second.vector);
    } else {
      spdlog::warn("Failed to populate quick map for {:s}", user.name);
    }

    if (occurrence.second) {
      load(user, occurrence.first->second.vector);
    } else {
      spdlog::warn("Failed to populate occurrence map for {:s}", user.name);
    }

  });
}

template <typename T>
void Storage::load(const User & user, std::vector<T> & vector) {
  FileHandle<std::ifstream> handle(user.name, TypeProps<T>::path);
  if (!handle.good()) return;

  std::string buffer;
  vector.clear();
  while (std::getline(handle.file, buffer)) {
    if (buffer.empty()) continue;

    auto entry = parseValue<T>(buffer);
    if (!entry) {
      spdlog::error("Malformed value entry {:s}", buffer);
      continue;
    }

    vector.emplace_back(std::move(*entry));
  }
}
