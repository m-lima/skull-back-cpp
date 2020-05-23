#pragma once

#include <fstream>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "constants.hpp"
#include "file_handle.hpp"
#include "format.hpp"
#include "model.hpp"

class Storage {
private:
  template <typename T>
  struct LockedVector {
    std::mutex mutex;
    std::vector<T> vector;

    LockedVector(std::vector<T> && vector) : vector{std::move(vector)} {}

    LockedVector(const std::vector<T> &) = delete;
    LockedVector(LockedVector &&) = default;
    LockedVector(const LockedVector &) = delete;
    LockedVector & operator=(const LockedVector &) = delete;
  };

  std::unordered_map<User, LockedVector<Skull>> mSkulls;
  std::unordered_map<User, LockedVector<Quick>> mQuicks;
  std::unordered_map<User, LockedVector<Occurrence>> mOccurrences;

  template <typename T>
  struct TypeProps {
  };

  template <typename T, typename S>
  static void stream(const std::vector<T> & vector, S & stream) {
    stream << '[';
    auto end = vector.cend() - 1;
    for (auto it = vector.cbegin(); it != end; ++it) {
      stream << format::json{*it} << ',';
    }
    stream << format::json{*end} << ']';
  }

  template <typename T>
  static void save(const std::string && user,
                   const std::vector<T> * const vector,
                   std::unique_lock<std::mutex> &&) {
    FileHandle<std::ofstream> handle(user, TypeProps<T>::path);
    if (!handle.good()) return;

    for (const auto & value : *vector) {
      handle.file << format::tsv{value} << '\n';
    }
  }

  template <typename T>
  static void load(const User & user, std::vector<T> & vector);

public:
  Storage();

  [[nodiscard]]
  inline bool authorized(const User & user) const {
    return user != constant::user::UNKNOWN && mSkulls.find(user) != mSkulls.cend();
  }

  template <typename T>
  [[nodiscard]]
  std::string get(const User & user) {
    const auto values = (this->*TypeProps<T>::map).find(user);
    if (values == (this->*TypeProps<T>::map).cend()) return "[]";
    if (values->second.vector.empty()) return "[]";

    std::lock_guard lock{values->second.mutex};

    std::stringstream output;
    stream(values->second.vector, output);
    return output.str();
  }

  template <typename T, typename S>
  void stream(const User & user, S & output) {
    const auto values = (this->*TypeProps<T>::map).find(user);
    if (values == (this->*TypeProps<T>::map).cend() || values->second.vector.empty()) {
      output << "[]";
      return;
    }

    std::lock_guard lock{values->second.mutex};

    stream(values->second.vector, output);
  }

  template <typename T>
  bool add(const User & user, T && value) {
    const auto values = (this->*TypeProps<T>::map).find(user);
    if (values == (this->*TypeProps<T>::map).cend()) return false;

    std::unique_lock lock{values->second.mutex};
    values->second.vector.emplace_back(std::forward<T>(value));

    std::thread saver{save<T>, std::string{user.name}, &(values->second.vector), std::move(lock)};
    saver.detach();
    return true;
  }

  template <typename T>
  bool remove(const User & user, T && value) {
    const auto values = (this->*TypeProps<T>::map).find(user);
    if (values == (this->*TypeProps<T>::map).cend()) return false;

    std::unique_lock lock{values->second.mutex};

    auto entry = std::find(values->second.vector.begin(), values->second.vector.end(), value);
    if (entry == values->second.vector.end()) return false;
    values->second.vector.erase(entry);

    std::thread saver{save<T>, std::string{user.name}, &(values->second.vector), std::move(lock)};
    saver.detach();
    return true;
  }

  bool reload(const User & user) {
    const auto skull = mSkulls.find(user);
    if (skull == mSkulls.cend()) return false;

    const auto quick = mQuicks.find(user);
    if (quick == mQuicks.cend()) return false;

    const auto occurrence = mOccurrences.find(user);
    if (occurrence == mOccurrences.cend()) return false;

    std::thread loader{[&](const std::string && user){
      std::lock_guard skullLock{skull->second.mutex};
      std::lock_guard quickLock{quick->second.mutex};
      std::lock_guard occurrenceLock{occurrence->second.mutex};

      load(user, skull->second.vector);
      load(user, quick->second.vector);
      load(user, occurrence->second.vector);
    }, std::string{user.name}};
    loader.detach();

    return true;
  }

  template <typename T>
  [[nodiscard]]
  inline std::size_t estimateSize(const User & user) const {
    const auto values = (this->*TypeProps<T>::map).find(user);
    if (values == (this->*TypeProps<T>::map).cend()) return 0;

    return values->second.vector.size() * 50;
  }
};

template <>
struct Storage::TypeProps<Skull> {
  static constexpr const auto & path = constant::file::SKULL;
  static constexpr auto Storage::* const map = &Storage::mSkulls;
};

template <>
struct Storage::TypeProps<Quick> {
  static constexpr const auto & path = constant::file::QUICK;
  static constexpr auto Storage::* const map = &Storage::mQuicks;
};

template <>
struct Storage::TypeProps<Occurrence> {
  static constexpr const auto & path = constant::file::OCCURRENCE;
  static constexpr auto Storage::* const map = &Storage::mOccurrences;
};
