#include "context.hpp"

#include <restinio/all.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"

namespace {
  std::atomic<std::uint16_t> COUNTER;

  inline std::string_view methodToString(restinio::http_method_t method) {
    switch (method) {
      case restinio::http_method_get(): return "GET";
      case restinio::http_method_post(): return "POST";
      case restinio::http_method_delete(): return "DELETE";
      case restinio::http_method_options(): return "OPTIONS";
      default: return "UNKNOWN";
    }
  }
}

Context::Context(restinio::request_handle_t request)
    : id{COUNTER++},
      request{request},
      user{request->header().has_field(constant::header::X_USER)
           ? request->header().get_field(constant::header::X_USER)
           : constant::user::UNKNOWN},
      root{[&]() -> std::optional<boost::filesystem::path> {
        if (user == constant::user::UNKNOWN) {
          return {};
        }

        try {
          auto userPath = boost::filesystem::path{constant::file::ROOT} / user.name;
          if (boost::filesystem::exists(userPath)) {
            return std::move(userPath);
          }
        } catch (...) {}

        return {};
      }()} {
  spdlog::info("{} {:s} {:s}",
               *this,
               methodToString(request->header().method()),
               request->header().request_target());
}

