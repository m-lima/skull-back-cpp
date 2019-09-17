#pragma once

#include <restinio/all.hpp>

#include "context.hpp"

namespace server {
  using Handler = restinio::request_handling_status_t;

  void listen(std::string && host, std::uint16_t port, std::uint16_t threadCount) noexcept;

  Handler getQuick(Context &&) noexcept;
  Handler postQuick(Context &&) noexcept;
  Handler deleteQuick(Context &&) noexcept;
  Handler getSkull(Context &&) noexcept;
  Handler postSkull(Context &&) noexcept;
  Handler deleteSkull(Context &&) noexcept;
  Handler reload(Context &&) noexcept;
}
