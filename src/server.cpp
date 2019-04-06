#include "server.hpp"

#include <fstream>
#include <memory>

#include <restinio/all.hpp>
#include <restinio/transforms/zlib.hpp>
#include <spdlog/spdlog.h>

#include "constants.hpp"
#include "context.hpp"
#include "response.hpp"
#include "storage.hpp"

namespace {
  Storage storage;

  struct ServerTraits : public restinio::default_single_thread_traits_t {
    using request_handler_t = restinio::router::express_router_t<>;
  };

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
  void listen(std::string && host, std::uint16_t port, std::uint16_t threadCount) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get(constant::path::GET_QUICK, [](auto request, auto) { return getQuick(request); });
    router->http_get(constant::path::GET_SKULL, [](auto request, auto) { return getSkull(request); });
    router->http_post(constant::path::POST_SKULL, [](auto request, auto) { return postSkull(request); });
    router->http_delete(constant::path::DELETE_SKULL, [](auto request, auto) { return deleteSkull(request); });
    router->non_matched_request_handler([](auto request) { return notFound(request); });

    if (threadCount < 2) {
      spdlog::info("Listening on {:s}:{:d} on a single thread..", host, port);

      restinio::run(restinio::on_this_thread<ServerTraits>()
                        .address(std::move(host))
                        .port(port)
                        .request_handler(std::move(router)));
    } else {
      spdlog::info("Listening on {:s}:{:d} on {:d} threads..", host, port, threadCount);

      restinio::run(restinio::on_thread_pool<ServerTraits>(threadCount)
                        .address(std::move(host))
                        .port(port)
                        .request_handler(std::move(router)));
    }
  }

  Handler getQuick(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      auto values = storage.getQuickValues(context.user);

      if (!values) return notFound(std::move(context));

      return Response{std::move(context), restinio::status_ok()}
          .appendHeader(restinio::http_field::content_type, "text/json; charset=utf-8")
          .setBody(*values)
          .done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler getSkull(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      auto values = storage.getSkullValues(context.user);

      if (!values) return notFound(std::move(context));

      return Response{std::move(context), restinio::status_ok()}
          .appendHeader(restinio::http_field::content_type, "text/json; charset=utf-8")
          .setBody(*values)
          .done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler postSkull(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::query::TYPE) || !query.has(constant::query::AMOUNT)) {
        return badRequest(std::move(context));
      }

      {
        using namespace std::chrono;

        SkullValue value{query[constant::query::TYPE],
                         query[constant::query::AMOUNT],
                         std::to_string(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count())};

        if (value.type().empty() || value.amount().empty()) {
          return badRequest(std::move(context));
        }

        if (value.type() == constant::query::UNDEFINED || value.amount() == constant::query::UNDEFINED) {
          return badRequest(std::move(context));
        }

        storage.addSkullValue(context.user, std::move(value));
      }

      return Response{std::move(context), restinio::status_created()}.done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler deleteSkull(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::query::TYPE)
          || !query.has(constant::query::AMOUNT)
          || !query.has(constant::query::MILLIS)) {
        return badRequest(std::move(context));
      }

      SkullValue value{query[constant::query::TYPE], query[constant::query::AMOUNT], query[constant::query::MILLIS]};

      if (value.type().empty() || value.amount().empty() || value.millis().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::query::UNDEFINED
          || value.amount() == constant::query::UNDEFINED
          || value.millis() == constant::query::UNDEFINED) {
        return badRequest(std::move(context));
      }

      storage.deleteSkullValue(context.user, value);

      return Response{std::move(context), restinio::status_accepted()}.done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }
}
