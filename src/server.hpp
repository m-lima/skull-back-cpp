#pragma once

#include <string>

#include <restinio/all.hpp>

#include "auth.hpp"

namespace server {
  using Request = restinio::request_handle_t;
  using ConstRequest = const std::shared_ptr<const restinio::request_t>;

  inline restinio::request_handling_status_t fail(Request request, restinio::http_status_line_t && status) noexcept {
    request->create_response(std::move(status)).connection_close().done();
    return restinio::request_rejected();
  }

  void listen(std::string && host,
              std::uint16_t port,
              std::string_view quickValuePath,
              std::string_view skullPath) noexcept;

  bool authorized(ConstRequest) noexcept;
  restinio::request_handling_status_t getQuick(Request, std::string_view quickValuesPath) noexcept;
  restinio::request_handling_status_t postSkull(Request, std::string_view skullPath) noexcept;
  restinio::request_handling_status_t getSkull(Request, std::string_view skullPath) noexcept;
}
