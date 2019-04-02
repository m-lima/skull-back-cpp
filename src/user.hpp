#pragma once

#include <string>

#include <mfl/string.hpp>

struct User {
  const char * const name;
  const std::size_t hash;

  constexpr explicit User(const char * const name)
      : name{name},
        hash{mfl::string::hash64::hash(name)} {}

  template <std::size_t N>
  constexpr User(const char (& name)[N])
      : name{name},
        hash{mfl::string::hash64::hash(name)} {}

  User(const std::string & name) : name{name.c_str()}, hash{mfl::string::hash64::hash(name)} {}

  inline bool operator==(const User & rhs) const {
    return hash == rhs.hash;
  }

  inline bool operator!=(const User & rhs) const {
    return hash != rhs.hash;
  }
};

template<>
struct std::hash<User> {
  inline auto operator()(const User & user) const {
    return user.hash;
  }
};
