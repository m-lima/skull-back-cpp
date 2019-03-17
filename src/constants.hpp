#pragma once

#include <array>

namespace constant {
  namespace path {
    constexpr const auto GET_QUICK = "/quick";
    constexpr const auto GET_SKULL = "/skull";
    constexpr const auto POST_SKULL = "/skull";
  }

  namespace file {
    constexpr const auto ROOT = "res";
    constexpr const auto QUICK = "quick_values.json";
    constexpr const auto SKULL = "skull.json";
  }

  namespace header {
    constexpr const auto X_USER = "X-User";
    constexpr const auto ORIGIN = "Origin";
  }

  namespace cors {
    constexpr const auto ALLOW_CORS = true;
    constexpr const auto ORIGIN = ".*";
  }

  namespace user {
    constexpr const auto UNKNOWN = "??";
  }
}
