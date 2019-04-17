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

  spdlog::level::level_enum levelForStatus(const restinio::http_status_line_t & status) {
    switch (status.status_code().raw_code() / 100) {
      case 1:
      case 2:
      case 3:return spdlog::level::info;
      case 4:return spdlog::level::warn;
      case 5:
      default:return spdlog::level::err;
    }
  }
}

Context::Context(restinio::request_handle_t && request)
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

template <>
void Context::logDone<true>(const restinio::http_status_line_t & status) const {
  spdlog::log(levelForStatus(status),
              "{} (stream) {:d} {:s}",
              *this,
              status.status_code().raw_code(),
              status.reason_phrase());
}

template <>
void Context::logDone<false>(const restinio::http_status_line_t & status) const {
  spdlog::log(levelForStatus(status),
              "{} {:d} {:s}",
              *this,
              status.status_code().raw_code(),
              status.reason_phrase());
}
