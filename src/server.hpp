#pragma once

#include <string>

#include <restinio/all.hpp>

#include "auth.hpp"

namespace server {
  inline restinio::request_handling_status_t fail(restinio::request_handle_t request,
                                                  restinio::http_status_line_t && status) {
    request->create_response(status).connection_close().done();
    return restinio::request_rejected();
  }

  inline restinio::request_handling_status_t forbidden(restinio::request_handle_t request) {
    return fail(request, restinio::status_forbidden());
  }

  inline restinio::request_handling_status_t badRequest(restinio::request_handle_t request) {
    return fail(request, restinio::status_bad_request());
  }

  inline restinio::request_handling_status_t internalServerError(restinio::request_handle_t request) {
    return fail(request, restinio::status_internal_server_error());
  }

  inline restinio::request_handling_status_t notFound(restinio::request_handle_t request) {
    return fail(request, restinio::status_not_found());
  }

  void start(std::string && host, std::uint16_t port, std::string_view quickValuePath, std::string_view skullPath);

  bool authorized(const std::shared_ptr<const restinio::request_t> request);
  restinio::request_handling_status_t getQuick(restinio::request_handle_t request, std::string_view quickValuesPath);
  restinio::request_handling_status_t postSkull(restinio::request_handle_t request, std::string_view skullPath);
  restinio::request_handling_status_t getSkull(restinio::request_handle_t request, std::string_view skullPath);
};



