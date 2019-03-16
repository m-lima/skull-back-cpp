#pragma once

#include <functional>
#include <string>

#include <boost/filesystem.hpp>
#include <restinio/all.hpp>

namespace server {
  using Request = restinio::request_handle_t;
  using ConstRequest = const std::shared_ptr<const restinio::request_t>;
  using Handler = restinio::request_handling_status_t;

  inline Handler fail(Request request, restinio::http_status_line_t && status) noexcept {
    request->create_response(std::move(status)).connection_close().done();
    return restinio::request_rejected();
  }

  void listen(std::string && host, std::uint16_t port) noexcept;

  std::optional<const boost::filesystem::path> authorize(ConstRequest request) noexcept;
  Handler makeContext(Request, std::function<Handler(std::size_t, Request)>) noexcept;
  Handler getQuick(std::size_t, Request) noexcept;
  Handler postSkull(std::size_t, Request) noexcept;
  Handler getSkull(std::size_t, Request) noexcept;
}
