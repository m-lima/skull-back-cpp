#pragma once

namespace constant {
  namespace path {
    constexpr const auto GET_QUICK = "/quick";
    constexpr const auto GET_SKULL = "/";
    constexpr const auto POST_SKULL = "/";
  }

  namespace file {
    constexpr const auto ROOT = "/data";
    constexpr const auto QUICK = "quick_values.json";
    constexpr const auto SKULL = "skull.json";
  }

  namespace header {
    constexpr const auto X_USER = "X-User";
  }

  namespace user {
    constexpr const auto UNKNOWN = "??";
  }
}

