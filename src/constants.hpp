#pragma once

#include "user.hpp"

namespace constant {
  namespace server {
    constexpr const auto MAX_BUFFER = 1024;
  }

  namespace path {
    constexpr const auto QUICK = "/quick";
    constexpr const auto SKULL = "/skull";
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
    constexpr const User UNKNOWN = "??";
  }

  namespace query {
    constexpr const auto UNDEFINED = "undefined";
    constexpr const auto TYPE = "type";
    constexpr const auto AMOUNT = "amount";
    constexpr const auto MILLIS = "millis";
    constexpr const auto ICON = "icon";
  }
}
