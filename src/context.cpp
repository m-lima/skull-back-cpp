#include "context.hpp"

#include <spdlog/spdlog.h>

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
           : constant::user::UNKNOWN} {
  spdlog::info("{} {:s} {:s}",
               *this,
               methodToString(request->header().method()),
               request->header().request_target());
}

void Context::logDone(const restinio::http_status_line_t & status) const {
  spdlog::log(status.status_code().raw_code() < 300 ? spdlog::level::info : spdlog::level::warn,
              "{} {:d} {:s}",
              *this,
              status.status_code().raw_code(),
              status.reason_phrase());
}

