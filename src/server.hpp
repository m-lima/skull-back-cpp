#pragma once

#include <restinio/all.hpp>

#include "context.hpp"

namespace server {
  using Handler = restinio::request_handling_status_t;

  void listen(std::string && host, std::uint16_t port, std::uint16_t threadCount) noexcept;

  Handler getQuick(Context &&) noexcept;
  Handler putQuick(Context &&) noexcept;
  Handler postQuick(Context &&, restinio::router::route_params_t &&) noexcept;
  Handler deleteQuick(Context &&, restinio::router::route_params_t &&) noexcept;

  Handler getSkull(Context &&) noexcept;
  Handler putSkull(Context &&) noexcept;
  Handler postSkull(Context &&, restinio::router::route_params_t &&) noexcept;
  Handler deleteSkull(Context &&, restinio::router::route_params_t &&) noexcept;
}
