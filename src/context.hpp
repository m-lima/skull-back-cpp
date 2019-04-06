#pragma once

#include <optional>

#include <restinio/all.hpp>

#include "user.hpp"

class Context {
public:
  const std::uint16_t id;
  const restinio::request_handle_t request;
  const User user;

  Context(restinio::request_handle_t request);
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
