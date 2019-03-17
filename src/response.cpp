#include "response.hpp"

#include <spdlog/spdlog.h>

namespace {
  const std::regex ORIGIN_REGEX{constant::cors::ORIGIN};
}

Response::Response(Context && context, restinio::http_status_line_t && status)
    : response{context.request->create_response(status)} {
  spdlog::log(status.status_code().raw_code() < 300 ? spdlog::level::info : spdlog::level::warn,
              "{} {:d} {:s}",
              context,
              status.status_code().raw_code(),
              status.reason_phrase());

  if (constant::cors::ALLOW_CORS) {
    if (!context.request->header().has_field(constant::header::ORIGIN)) {
      return;
    }

    auto origin = context.request->header().get_field(constant::header::ORIGIN);

    if (std::regex_match(origin, ORIGIN_REGEX)) {
      response.append_header("Access-Control-Allow-Origin", std::move(origin));
      response.append_header("Access-Control-Allow-Credentials", "true");
    }
  }
}

