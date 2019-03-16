#include "server.hpp"

#include <fstream>
#include <memory>

#include <mfl/out.hpp>
#include <mfl/string.hpp>
#include <restinio/all.hpp>
#include <restinio/transforms/zlib.hpp>

#include "constants.hpp"

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

  inline restinio::request_handling_status_t forbidden(server::Request request) noexcept {
    if (request->header().has_field(constant::header::X_USER)) {
      mfl::out::println(stderr,
          "403 Forbidden: {:s} {:s}",
          request->header().get_field(constant::header::X_USER),
          request->header().request_target());
    } else {
      mfl::out::println(stderr, "403 Forbidden: {:s}", request->header().request_target());
    }
    return server::fail(request, restinio::status_forbidden());
  }

  inline restinio::request_handling_status_t badRequest(server::Request request) noexcept {
    mfl::out::println(stderr, "400 Bad Request: {:s}", request->header().request_target());
    return server::fail(request, restinio::status_bad_request());
  }

  inline restinio::request_handling_status_t internalServerError(server::Request request) noexcept {
    mfl::out::println(stderr, "500 Internal Server Error: {:s}", request->header().request_target());
    return server::fail(request, restinio::status_internal_server_error());
  }

  inline restinio::request_handling_status_t notFound(server::Request request) noexcept {
    mfl::out::println(stderr, "404 Not Found: {:s}", request->header().request_target());
    return server::fail(request, restinio::status_not_found());
  }
}

namespace server {
  void listen(std::string && host, std::uint16_t port) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get(constant::path::GET_QUICK, [](auto request, auto) { return getQuick(request); });
    router->http_get(constant::path::GET_SKULL, [](auto request, auto) { return getSkull(request); });
    router->http_post(constant::path::POST_SKULL, [](auto request, auto) { return postSkull(request); });
    router->non_matched_request_handler(notFound);

    mfl::out::println("Listening on {:s}:{:d}..", host, port);

    restinio::run(restinio::on_this_thread<ServerTraits>()
                      .address(std::move(host))
                      .port(port)
                      .request_handler(std::move(router)));
  }

  restinio::request_handling_status_t getQuick(Request request) noexcept {
    try {
      auto user = authorize(request);
      if (!user) {
        return forbidden(request);
      }

      auto path = *user / constant::file::QUICK;

      std::ifstream quick(path.c_str());
      if (!quick.good()) {
        return notFound(request);
      }
      quick.close();

      request->create_response(restinio::status_created())
          .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
          .set_body(restinio::sendfile(path.string()))
          .done();
      return restinio::request_accepted();
    } catch (...) {
      mfl::out::println(stderr, "Exception caught", request->header().request_target());
      return internalServerError(request);
    }
  }

  restinio::request_handling_status_t postSkull(Request request) noexcept {
    try {
      auto user = authorize(request);
      if (!user) {
        return forbidden(request);
      }

      const auto query = restinio::parse_query(request->header().query());
      if (!query.has("type") || !query.has("value")) {
        return badRequest(request);
      }

      std::fstream skull = createOrOpen(*user / constant::file::SKULL);

      if (!skull.is_open()) {
        mfl::out::println(stderr, "Could not open file", request->header().request_target());
        return internalServerError(request);
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
      return restinio::request_accepted();
    } catch (...) {
      mfl::out::println(stderr, "Exception caught", request->header().request_target());
      return internalServerError(request);
    }
  }

  restinio::request_handling_status_t getSkull(Request request) noexcept {
    try {
      auto user = authorize(request);
      if (!user) {
        return forbidden(request);
      }

      auto path = *user / constant::file::SKULL;

      std::ifstream quick(path.c_str());
      if (!quick.good()) {
        return notFound(request);
      }
      quick.close();

      request->create_response(restinio::status_created())
          .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
          .set_body(restinio::sendfile(path.string()))
          .done();

      return restinio::request_accepted();
    } catch (...) {
      mfl::out::println(stderr, "Exception caught", request->header().request_target());
      return internalServerError(request);
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
}
