#pragma once

#include <sstream>

#include "constants.hpp"

template <typename T, typename C>
class Response {
  friend class Context;

  restinio::response_builder_t<T> response;
  const C callback;
  std::stringstream buffer;

  Response(restinio::response_builder_t<T> && response, C && callback)
      : response{std::move(response)},
        callback{std::move(callback)} {}

  Response(const Response &) = delete;
  Response & operator=(const Response &) = delete;

public:
  Response(Response && response) : response{std::move(response.response)}, callback{std::move(response.callback)} {}

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

  inline Response && appendChunk(restinio::writable_item_t chunk) && {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.append_chunk(std::move(chunk));
    return std::move(*this);
  }

  inline Response && flush() && {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.flush();
    return std::move(*this);
  }

  inline Response & appendChunk(restinio::writable_item_t chunk) & {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.append_chunk(std::move(chunk));
    return *this;
  }

  inline Response & flush() & {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.flush();
    return *this;
  }

  inline restinio::request_handling_status_t done() {
    if constexpr (std::is_same_v<T, restinio::chunked_output_t>) {
      if (buffer.tellp() >= 0) {
        response.append_chunk(buffer.str());
      }
    }

    callback(response.header().status_line());
    return response.done();
  }

  template <typename V>
  friend inline Response & operator<<(Response<T, C> & response, V && value) {
    static_assert(std::is_same_v<T, restinio::chunked_output_t>);
    response.buffer << std::forward<V>(value);
    if (response.buffer.tellp() > constant::server::MAX_BUFFER) {
      response.response.append_chunk(response.buffer.str());
      response.response.flush();
      response.buffer.str(std::string{});
    }

    return response;
  }
};
