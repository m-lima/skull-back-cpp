#include "server.hpp"

#include <fstream>
#include <memory>

#include <mfl/string.hpp>
#include <restinio/all.hpp>
#include <restinio/transforms/zlib.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"

namespace {
  std::atomic<std::size_t> COUNTER;

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

  inline server::Handler forbidden(std::size_t id, server::Request request) noexcept {
    if (request->header().has_field(constant::header::X_USER)) {
      spdlog::warn("[{:0>8}] 403 Forbidden: {:s}", id, request->header().get_field(constant::header::X_USER));
    } else {
      spdlog::warn("[{:0>8}] 403 Forbidden", id);
    }
    return server::fail(request, restinio::status_forbidden());
  }

  inline server::Handler badRequest(std::size_t id, server::Request request) noexcept {
    spdlog::warn("[{:0>8}] 400 Bad Request", id);
    return server::fail(request, restinio::status_bad_request());
  }

  inline server::Handler internalServerError(std::size_t id, server::Request request) noexcept {
    spdlog::warn("[{:0>8}] 500 Internal Server Error", id);
    return server::fail(request, restinio::status_internal_server_error());
  }

  inline server::Handler notFound(std::size_t id, server::Request request) noexcept {
    spdlog::warn("[{:0>8}] 404 Not Found", id);
    return server::fail(request, restinio::status_not_found());
  }

  inline std::string_view methodToString(restinio::http_method_t method) {
    switch (method) {
      case restinio::http_method_get(): return "GET";
      case restinio::http_method_post(): return "POST";
      default: return "UNKNOWN";
    }
  }
}

namespace server {
  void listen(std::string && host, std::uint16_t port) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get(constant::path::GET_QUICK, [](auto request, auto) { return makeContext(request, &getQuick); });
    router->http_get(constant::path::GET_SKULL, [](auto request, auto) { return makeContext(request, &getSkull); });
    router->http_post(constant::path::POST_SKULL, [](auto request, auto) { return makeContext(request,&postSkull); });
    router->non_matched_request_handler([] (auto request) { return notFound(std::rand(), request); });

    spdlog::info("Listening on {:s}:{:d}..", host, port);

    restinio::run(restinio::on_this_thread<ServerTraits>()
                      .address(std::move(host))
                      .port(port)
                      .request_handler(std::move(router)));
  }

  Handler getQuick(std::size_t id, Request request) noexcept {
    try {
      auto user = authorize(request);
      if (!user) {
        return forbidden(id, request);
      }

      auto path = *user / constant::file::QUICK;

      std::ifstream quick(path.c_str());
      if (!quick.good()) {
        return notFound(id, request);
      }
      quick.close();

      request->create_response(restinio::status_created())
          .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
          .set_body(restinio::sendfile(path.string()))
          .done();

      spdlog::info("[{:0>8}] 200 OK", id);
      return restinio::request_accepted();
    } catch (const std::exception & e) {
      spdlog::error("[{:0>8}] Exception: {:s}", id, e.what());
      return internalServerError(id, request);
    }
  }

  Handler getSkull(std::size_t id, Request request) noexcept {
    try {
      auto user = authorize(request);
      if (!user) {
        return forbidden(id, request);
      }

      auto path = *user / constant::file::SKULL;

      std::ifstream quick(path.c_str());
      if (!quick.good()) {
        return notFound(id, request);
      }
      quick.close();

      request->create_response(restinio::status_created())
          .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
          .set_body(restinio::sendfile(path.string()))
          .done();

      spdlog::info("[{:0>8}] 200 OK", id);
      return restinio::request_accepted();
    } catch (const std::exception & e) {
      spdlog::error("[{:0>8}] Exception: {:s}", id, e.what());
      return internalServerError(id, request);
    }
  }

  Handler postSkull(std::size_t id, Request request) noexcept {
    try {
      auto user = authorize(request);
      if (!user) {
        return forbidden(id, request);
      }

      const auto query = restinio::parse_query(request->header().query());
      if (!query.has("type") || !query.has("value")) {
        return badRequest(id, request);
      }

      std::fstream skull = createOrOpen(*user / constant::file::SKULL);

      if (!skull.is_open()) {
        spdlog::error("[{:0>8}] Could not open file", id);
        return internalServerError(id, request);
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

      request->create_response(restinio::status_created()).done();
      spdlog::info("[{:0>8}] 200 OK", id);
      return restinio::request_accepted();
    } catch (const std::exception & e) {
      spdlog::error("[{:0>8}] Exception: {:s}", id, e.what());
      return internalServerError(id, request);
    }
  }

  std::optional<const boost::filesystem::path> authorize(ConstRequest request) noexcept {
    if (!request->header().has_field(constant::header::X_USER)) {
      return {};
    }

    try {
      auto userHeader = request->header().get_field(constant::header::X_USER);
      auto userPath = boost::filesystem::path{constant::file::ROOT} / userHeader;
      if (boost::filesystem::exists(userPath)) {
        return std::move(userPath);
      }
    } catch (...) {}

    return {};
  }

  Handler makeContext(Request request, std::function<Handler(std::size_t, Request)> handler) noexcept {
    auto id = COUNTER++;
    spdlog::info("[{:0>8}] Start {:s} {:s}",
        id,
        methodToString(request->header().method()),
        request->header().request_target());
    auto response = handler(id, request);
    spdlog::info("[{:0>8}] End {:s} {:s}",
        id,
        methodToString(request->header().method()),
        request->header().request_target());
    return response;
  }
}
