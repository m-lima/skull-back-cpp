#include "file_handle.hpp"

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"

template <>
FileHandle<std::ofstream>::FileHandle(const std::string & user, const char * const fileName)
    : path{(boost::filesystem::path{constant::file::ROOT} / user / fileName).generic_string()},
      file{path} {
  if (!file.good()) {
    file.close();
    spdlog::error("Failed to open {:s} for saving", path);
  }
}

template <>
FileHandle<std::ofstream>::~FileHandle() {
  if (file.is_open()) spdlog::info("Updated {:s}", path);
  file.close();
}

template <>
FileHandle<std::ifstream>::FileHandle(const std::string & user, const char * const fileName)
    : path{(boost::filesystem::path{constant::file::ROOT} / user / fileName).generic_string()},
      file{path} {
  if (!file.good()) {
    file.close();
    spdlog::error("Failed to open {:s} for loading", path);
  }
}

template <>
FileHandle<std::ifstream>::~FileHandle() {
  if (file.is_open()) spdlog::info("Loaded {:s}", path);
  file.close();
}

void UserIterator::forEach(std::function<void(const User &)> executor) {
  auto root = boost::filesystem::path{constant::file::ROOT};

  for (auto it{boost::filesystem::directory_iterator{root}}; it != boost::filesystem::directory_iterator{}; ++it) {
    auto user = it->path().filename().generic_string();
    spdlog::info("Found user: {:s}", user);
    executor(user);
  }
}
