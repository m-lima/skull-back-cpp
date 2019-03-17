#pragma once

#include <regex>

#include <restinio/all.hpp>

#include "constants.hpp"
#include "context.hpp"

class Response {
  restinio::response_builder_t<restinio::restinio_controlled_output_t> response;
  const std::regex ORIGIN_REGEX{constant::cors::ORIGIN};

public:
  Response(Context &&, restinio::http_status_line_t &&);

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
