#pragma once

#include "user.hpp"

namespace constant {
  namespace server {
    constexpr auto const MAX_BUFFER = 64 * 1024;
  }

  namespace path {
    constexpr char const QUICK[] = "/quick/";
    constexpr char const SKULL[] = "/";
  }

  namespace file {
    constexpr char const ROOT[] = "/data";
    constexpr char const QUICK[] = "quick";
    constexpr char const SKULL[] = "skull";
  }

  namespace header {
    constexpr char const X_USER[] = "X-User";
  }

  namespace user {
    constexpr User const UNKNOWN = "??";
  }

  namespace param {
    constexpr char const UNDEFINED[] = "undefined";
    constexpr char const TYPE[] = "type";
    constexpr char const AMOUNT[] = "amount";
    constexpr char const MILLIS[] = "millis";
    constexpr char const ICON[] = "icon";
    constexpr char const ID[] = "id";
  }
}
