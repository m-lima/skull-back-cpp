#include "server.hpp"

#include <fstream>
#include <memory>

#include <mfl/string.hpp>
#include <restinio/all.hpp>
#include <restinio/transforms/zlib.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"
#include "context.hpp"
#include "response.hpp"

namespace {
  struct ServerTraits : public restinio::default_single_thread_traits_t {
    using request_handler_t = restinio::router::express_router_t<>;
  };

  inline auto createOrOpen(const boost::filesystem::path & path) noexcept {
    std::fstream file;

    boost::filesystem::create_directories(path.parent_path());

    file.open(path.c_str(), std::ios_base::out | std::ios_base::in);
    if (file.is_open()) {
      file.seekg(0, file.end);
      if (file.tellg() == 0) {
        file << '[';
      } else {
        file.seekg(-1, std::ios_base::end);
        file << ',';
      }
    } else {
      file.open(path.c_str(), std::ios_base::app);
      if (file.is_open()) {
        file << '[';
      }
    }

    return file;
  }

  inline server::Handler forbidden(Context && context) noexcept {
    return server::fail(std::move(context), restinio::status_forbidden());
  }

  inline server::Handler badRequest(Context && context) noexcept {
    return server::fail(std::move(context), restinio::status_bad_request());
  }

  inline server::Handler internalServerError(Context && context) noexcept {
    return server::fail(std::move(context), restinio::status_internal_server_error());
  }

  inline server::Handler notFound(Context && context) noexcept {
    return server::fail(std::move(context), restinio::status_not_found());
  }
}

namespace server {
  void listen(std::string && host, std::uint16_t port) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get(constant::path::GET_QUICK, [](auto request, auto) { return getQuick(request); });
    router->http_get(constant::path::GET_SKULL, [](auto request, auto) { return getSkull(request); });
    router->http_post(constant::path::POST_SKULL, [](auto request, auto) { return postSkull(request); });
    router->add_handler(restinio::http_method_options(), "/", [](auto request, auto) { return preflight(request)});
    router->non_matched_request_handler([] (auto request) { return notFound(request); });

    spdlog::info("Listening on {:s}:{:d}..", host, port);

    restinio::run(restinio::on_this_thread<ServerTraits>()
                      .address(std::move(host))
                      .port(port)
                      .request_handler(std::move(router)));
  }

  Handler preflight(Context && context) noexcept {
    return Response{std::move(context), restinio::status_ok()}.done();
  }

  Handler getQuick(Context && context) noexcept {
    try {
      if (!context.authorized()) return forbidden(std::move(context));

      auto path = *context.root / constant::file::QUICK;

      if (!boost::filesystem::exists(path)) {
        return notFound(std::move(context));
      }

      return Response{std::move(context), restinio::status_ok()}
          .appendHeader(restinio::http_field::content_type, "text/json; charset=utf-8")
          .setBody(restinio::sendfile(path.string()))
          .done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler getSkull(Context && context) noexcept {
    try {
      if (!context.authorized()) return forbidden(std::move(context));

      auto path = *context.root / constant::file::SKULL;

      if (!boost::filesystem::exists(path)) {
        return notFound(std::move(context));
      }

      return Response{std::move(context), restinio::status_ok()}
          .appendHeader(restinio::http_field::content_type, "text/json; charset=utf-8")
          .setBody(restinio::sendfile(path.string()))
          .done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler postSkull(Context && context) noexcept {
    try {
      if (!context.authorized()) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has("type") || !query.has("value")) {
        return badRequest(std::move(context));
      }

      std::fstream skull = createOrOpen(*context.root / constant::file::SKULL);

      if (!skull.is_open()) {
        spdlog::error("{} Could not open file", context);
        return internalServerError(std::move(context));
      }

      {
        using namespace std::chrono;
        fmt::print(skull,
                   R"-({{"type":"{:s}","value":{:s},"millis":{:d}}}])-",
                   query["type"],
                   query["value"],
                   duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
      }
      skull.close();

      // ZLib skull.json

      return Response{std::move(context), restinio::status_created()}.done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }
}
