#pragma once

#include <string>

#include <boost/filesystem.hpp>
#include <restinio/all.hpp>

#include "auth.hpp"

namespace server {
  using Request = restinio::request_handle_t;
  using ConstRequest = const std::shared_ptr<const restinio::request_t>;

  inline restinio::request_handling_status_t fail(Request request, restinio::http_status_line_t && status) noexcept {
    request->create_response(std::move(status)).connection_close().done();
    return restinio::request_rejected();
  }

  void listen(std::string && host, std::uint16_t port) noexcept;

  std::optional<const boost::filesystem::path> authorize(ConstRequest request) noexcept;
  restinio::request_handling_status_t getQuick(Request) noexcept;
  restinio::request_handling_status_t postSkull(Request) noexcept;
  restinio::request_handling_status_t getSkull(Request) noexcept;
}
