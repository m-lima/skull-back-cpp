#pragma once

#include <functional>
#include <string>

#include <boost/filesystem.hpp>
#include <restinio/all.hpp>

#include "context.hpp"
#include "response.hpp"

namespace server {
  using Handler = restinio::request_handling_status_t;

  inline Handler fail(Context && context, restinio::http_status_line_t && status) noexcept {
    return Response{std::move(context), std::move(status)}.connectionClose().done();
  }

  void listen(std::string && host, std::uint16_t port) noexcept;

  Handler preflight(Context &&) noexcept;
  Handler getQuick(Context &&) noexcept;
  Handler postSkull(Context &&) noexcept;
  Handler getSkull(Context &&) noexcept;
}
