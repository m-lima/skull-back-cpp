#include "server.hpp"

#include <fstream>
#include <memory>

#include <mfl/out.hpp>
#include <mfl/string.hpp>
#include <restinio/all.hpp>

namespace {
  struct ServerTraits : public restinio::default_single_thread_traits_t {
    using request_handler_t = restinio::router::express_router_t<>;
  };

  inline auto createOrOpen(std::string_view path) {
    std::fstream file;

    file.open(&path.front(), std::ios_base::out | std::ios_base::in);
    if (file.is_open()) {
      file.seekg(0, file.end);
      if (file.tellg() == 0) {
        file << '[';
      } else {
        file.seekg(-1, std::ios_base::end);
        file << ',';
      }
    } else {
      file.open(&path.front(), std::ios_base::app);
      if (file.is_open()) {
        file << '[';
      }
    }

    return file;
  }
}

namespace server {
  void start(std::string && host, std::uint16_t port, std::string_view quickValuePath, std::string_view skullPath) {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get("/quick", [quickValuePath](auto request, auto) { return getQuick(request, quickValuePath); });
    router->http_post("/skull", [skullPath](auto request, auto) { return postSkull(request, skullPath); });
    router->http_get("/skull", [skullPath](auto request, auto) { return getSkull(request, skullPath); });
    router->non_matched_request_handler(server::notFound);

    mfl::out::println("Listening on {:s}:{:d}..", host, port);

    restinio::run(restinio::on_this_thread<ServerTraits>()
                      .address(std::move(host))
                      .port(port)
                      .request_handler(std::move(router)));
  }

  restinio::request_handling_status_t getQuick(restinio::request_handle_t request, std::string_view quickValuesPath) {
    if (!authorized(request)) {
      return forbidden(request);
    }

    request->create_response(restinio::status_created())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(restinio::sendfile(quickValuesPath))
        .done();
    return restinio::request_accepted();
  }

  restinio::request_handling_status_t postSkull(restinio::request_handle_t request, std::string_view skullPath) {
    if (!authorized(request)) {
      return forbidden(request);
    }

    const auto query = restinio::parse_query(request->header().query());
    if (!query.has("type") || !query.has("value")) {
      return badRequest(request);
    }

    std::fstream skull = createOrOpen(skullPath);

    if (!skull.is_open()) {
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

    request->create_response(restinio::status_created()).done();
    return restinio::request_accepted();
  }

  restinio::request_handling_status_t getSkull(restinio::request_handle_t request, std::string_view skullPath) {
    if (!authorized(request)) {
      return forbidden(request);
    }

    request->create_response(restinio::status_created())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(restinio::sendfile(skullPath))
        .done();

    return restinio::request_accepted();
  }

  bool authorized(const std::shared_ptr<const restinio::request_t> request) {
    if (!request->header().has_field("Cookie")) {
      return false;
    }

    auto cookieHeader = request->header().get_field("Cookie");
    auto startIndex = cookieHeader.find("session=");

    if (startIndex == std::string::npos) {
      return false;
    }
    startIndex += 8;

    auto endIndex = cookieHeader.find(';', startIndex);
    if (endIndex == std::string::npos) {
      endIndex = cookieHeader.size();
    }

    std::string session{cookieHeader.cbegin() + startIndex, cookieHeader.cbegin() + endIndex};

    if (session.empty()) {
      return false;
    }

    return true;
  }
}
