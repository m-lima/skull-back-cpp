#pragma once

#include <optional>

#include <boost/filesystem.hpp>
#include <restinio/all.hpp>

class Context {
public:
  const std::uint16_t id;
  const restinio::request_handle_t request;
  const std::string user;
  const std::optional<const boost::filesystem::path> root;

  Context(restinio::request_handle_t request);

  inline bool authorized() const {
    return root.has_value();
  }
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
      return format_to(ctx.begin(), "[{:0>5}] ({:s})", context.id, context.user);
    }
  };
}
