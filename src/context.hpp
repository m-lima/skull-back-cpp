#pragma once

#include <restinio/all.hpp>

#include "response.hpp"

class Context {
public:
  const std::uint16_t id;
  const restinio::request_handle_t request;
  const User user;

  Context(const restinio::request_handle_t & request);

  template <typename T = restinio::restinio_controlled_output_t>
  auto createResponse(restinio::http_status_line_t && status) {
    return Response{request->create_response<T>(std::move(status))
#ifdef LOCAL_DEVELOPMENT
        .append_header(restinio::http_field::access_control_allow_origin, request->header().get_field(restinio::http_field::origin))
        .append_header(restinio::http_field::access_control_allow_headers, "x-user")
        .append_header(restinio::http_field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS")
        .append_header(restinio::http_field::access_control_allow_credentials, "true"),
#else
,
#endif
                    [this](const restinio::http_status_line_t & status) {
                      logDone<std::is_same_v<T, restinio::chunked_output_t>>(status);
                    }};
  }

private:
  template <bool streamed>
  void logDone(const restinio::http_status_line_t & status) const;
};

namespace fmt {
  template <>
  struct formatter<Context> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext & ctx) {
      return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Context & context, FormatContext & ctx) {
      return format_to(ctx.begin(), "[{:0>5}] ({:s})", context.id, context.user.name);
    }
  };
}
