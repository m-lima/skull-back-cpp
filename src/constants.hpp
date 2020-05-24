#pragma once

#include "user.hpp"

namespace constant {
  namespace server {
    constexpr const auto MAX_BUFFER = 64 * 1024;
  }

  namespace path {
    constexpr const auto SKULL = "/skull";
    constexpr const auto QUICK = "/quick";
    constexpr const auto OCCURRENCE = "/occurrence";
    constexpr const auto RELOAD = "/reload";
  }

  namespace file {
    constexpr const auto ROOT = "/data";
    constexpr const auto SKULL = "skull";
    constexpr const auto QUICK = "quick";
    constexpr const auto OCCURRENCE = "occurrence";
  }

  namespace header {
    constexpr const auto X_USER = "X-User";
  }

  namespace user {
    constexpr const User UNKNOWN = "??";
  }

  namespace query {
    constexpr const auto UNDEFINED = "undefined";
    constexpr const auto ID = "id";
    constexpr const auto NAME = "name";
    constexpr const auto COLOR = "color";
    constexpr const auto ICON = "icon";
    constexpr const auto UNIT_PRICE = "unitPrice";
    constexpr const auto LIMIT = "limit";
    constexpr const auto SKULL = "skull";
    constexpr const auto AMOUNT = "amount";
    constexpr const auto MILLIS = "millis";
  }
}
