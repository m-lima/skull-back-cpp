#pragma once

#include <restinio/all.hpp>

#include "context.hpp"

namespace server {
  using Handler = restinio::request_handling_status_t;

  void listen(std::string && host, std::uint16_t port, std::uint16_t threadCount) noexcept;

  Handler getSkull(Context &&) noexcept;
  Handler postSkull(Context &&) noexcept;
  Handler deleteSkull(Context &&) noexcept;
  Handler getQuick(Context &&) noexcept;
  Handler postQuick(Context &&) noexcept;
  Handler deleteQuick(Context &&) noexcept;
  Handler getOccurrence(Context &&) noexcept;
  Handler postOccurrence(Context &&) noexcept;
  Handler deleteOccurrence(Context &&) noexcept;
  Handler reload(Context &&) noexcept;
}
