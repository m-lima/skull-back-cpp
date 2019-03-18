#pragma once

#include <restinio/all.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"
#include "context.hpp"

class Response {
  restinio::response_builder_t<restinio::restinio_controlled_output_t> response;

public:
  Response(Context && context, restinio::http_status_line_t && status)
      : response{context.request->create_response(status)} {
    spdlog::log(status.status_code().raw_code() < 300 ? spdlog::level::info : spdlog::level::warn,
                "{} {:d} {:s}",
                context,
                status.status_code().raw_code(),
                status.reason_phrase());
  }

  inline Response && appendHeader(restinio::http_field_t field, std::string && value) && {
    response.append_header(field, std::move(value));
    return std::move(*this);
  }

  inline Response && appendHeader(std::string && field, std::string && value) && {
    response.append_header(std::move(field), std::move(value));
    return std::move(*this);
  }

  inline Response && setBody(restinio::writable_item_t body) && {
    response.set_body(std::move(body));
    return std::move(*this);
  }

  inline Response && connectionClose() && {
    response.connection_close();
    return std::move(*this);
  }

  inline restinio::request_handling_status_t done() {
    return response.done();
  }

};
