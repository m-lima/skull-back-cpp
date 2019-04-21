#include "server.hpp"

#include <mfl/string.hpp>
#include <spdlog/spdlog.h>

#include "storage.hpp"

namespace {
  Storage storage;

  struct ServerMode {
    using SingleThread = asio::executor;
    using MultiThread = asio::strand<asio::executor>;
  };

  template <typename Strand>
  using ServerTraits = restinio::traits_t<restinio::asio_timer_manager_t,
      restinio::null_logger_t,
      restinio::router::express_router_t<>,
      Strand>;

  inline server::Handler fail(Context && context, restinio::http_status_line_t && status) noexcept {
    return context.createResponse(std::move(status)).connectionClose().done();
  }

  inline server::Handler forbidden(Context && context) noexcept {
    return fail(std::move(context), restinio::status_forbidden());
  }

  inline server::Handler badRequest(Context && context) noexcept {
    return fail(std::move(context), restinio::status_bad_request());
  }

  inline server::Handler internalServerError(Context && context) noexcept {
    return fail(std::move(context), restinio::status_internal_server_error());
  }

  inline server::Handler notFound(Context && context) noexcept {
    return fail(std::move(context), restinio::status_not_found());
  }

  template <typename T>
  inline bool isNumberString(const T & string) {
    for (const auto & c : string) {
      if (c < '0' || c > '9') return false;
    }
    return true;
  }

  template <typename T>
  server::Handler getOrStream(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      if (storage.estimateSize<T>(context.user) < constant::server::MAX_BUFFER) {
        return context.createResponse(restinio::status_ok())
            .appendHeader(restinio::http_field::content_type, "text/json; charset=utf-8")
            .setBody(storage.get<T>(context.user))
            .done();
      }

      auto response = context.createResponse<restinio::chunked_output_t>(restinio::status_ok())
          .appendHeader(restinio::http_field::content_type, "text/json; charset=utf-8");

      storage.stream<T>(context.user, response);
      return response.done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  constexpr auto const QUICK_ID =
      mfl::string::Literal{constant::path::QUICK} + ":" + constant::param::ID + R""((\d+))"";
  constexpr auto const SKULL_ID =
      mfl::string::Literal{constant::path::SKULL} + ":" + constant::param::ID + R""((\d+))"";
}

namespace server {
  void listen(std::string && host, std::uint16_t port, std::uint16_t threadCount) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get(constant::path::QUICK, [](auto request, auto) {
      return getQuick(std::move(request));
    });
    router->http_put(constant::path::QUICK, [](auto request, auto) {
      return putQuick(std::move(request));
    });
    router->http_post(QUICK_ID.string, [](auto request, auto params) {
      return postQuick(std::move(request), std::move(params));
    });
    router->http_delete(QUICK_ID.string, [](auto request, auto params) {
      return deleteQuick(std::move(request), std::move(params));
    });

    router->http_get(constant::path::SKULL, [](auto request, auto) {
      return getSkull(std::move(request));
    });
    router->http_put(constant::path::SKULL, [](auto request, auto) {
      return putSkull(std::move(request));
    });
    router->http_post(SKULL_ID.string, [](auto request, auto params) {
      return postSkull(std::move(request), std::move(params));
    });
    router->http_delete(SKULL_ID.string, [](auto request, auto params) {
      return deleteSkull(std::move(request), std::move(params));
    });

    router->non_matched_request_handler([](auto request) {
      return notFound(std::move(request));
    });

    if (threadCount < 2) {
      spdlog::info("Listening on {:s}:{:d} on a single thread..", host, port);

      restinio::run(restinio::on_this_thread<ServerTraits<ServerMode::SingleThread>>()
                        .address(std::move(host))
                        .port(port)
                        .request_handler(std::move(router)));
    } else {
      spdlog::info("Listening on {:s}:{:d} on {:d} threads..", host, port, threadCount);

      restinio::run(restinio::on_thread_pool<ServerTraits<ServerMode::MultiThread>>(threadCount)
                        .address(std::move(host))
                        .port(port)
                        .request_handler(std::move(router)));
    }
  }

  Handler getQuick(Context && context) noexcept {
    return getOrStream<QuickValue>(std::move(context));
  }

  Handler putQuick(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::param::TYPE)
          || !query.has(constant::param::AMOUNT)
          || !query.has(constant::param::ICON)) {
        return badRequest(std::move(context));
      }

      QuickValue value{query[constant::param::TYPE], query[constant::param::AMOUNT], query[constant::param::ICON]};

      if (value.type().empty() || value.amount().empty() || value.icon().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::param::UNDEFINED
          || value.amount() == constant::param::UNDEFINED
          || value.icon() == constant::param::UNDEFINED) {
        return badRequest(std::move(context));
      }

      return storage.add(context.user, std::move(value))
             ? context.createResponse(restinio::status_created()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler postQuick(Context && context, restinio::router::route_params_t && params) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      auto id = restinio::cast_to<std::uint64_t>(params[constant::param::ID]);

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::param::TYPE)
          || !query.has(constant::param::AMOUNT)
          || !query.has(constant::param::ICON)) {
        return badRequest(std::move(context));
      }

      QuickValue value{query[constant::param::TYPE],
                       query[constant::param::AMOUNT],
                       query[constant::param::ICON]};

      if (value.type().empty() || value.amount().empty() || value.icon().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::param::UNDEFINED
          || value.amount() == constant::param::UNDEFINED
          || value.icon() == constant::param::UNDEFINED) {
        return badRequest(std::move(context));
      }

      return storage.edit(context.user, std::move(value), id)
             ? context.createResponse(restinio::status_created()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler deleteQuick(Context && context, restinio::router::route_params_t && params) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      auto id = restinio::cast_to<std::uint64_t>(params[constant::param::ID]);

      return storage.remove<QuickValue>(context.user, id)
             ? context.createResponse(restinio::status_accepted()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler getSkull(Context && context) noexcept {
    return getOrStream<SkullValue>(std::move(context));
  }

  Handler putSkull(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::param::TYPE) || !query.has(constant::param::AMOUNT)) {
        return badRequest(std::move(context));
      }

      auto millis = query.has(constant::param::MILLIS)
                    ? query[constant::param::MILLIS]
                    : std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch()).count());

      SkullValue value{query[constant::param::TYPE], query[constant::param::AMOUNT], millis};

      if (!isNumberString(value.millis())) {
        spdlog::warn("{} Could not parse '{:s}'", context, value.millis());
        return badRequest(std::move(context));
      }

      if (value.type().empty() || value.amount().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::param::UNDEFINED || value.amount() == constant::param::UNDEFINED) {
        return badRequest(std::move(context));
      }

      return storage.add(context.user, std::move(value))
             ? context.createResponse(restinio::status_created()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler postSkull(Context && context, restinio::router::route_params_t && params) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      auto id = restinio::cast_to<std::uint64_t>(params[constant::param::ID]);

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::param::TYPE)
          || !query.has(constant::param::AMOUNT)
          || !query.has(constant::param::MILLIS)) {
        return badRequest(std::move(context));
      }

      SkullValue value{query[constant::param::TYPE],
                       query[constant::param::AMOUNT],
                       query[constant::param::MILLIS]};

      if (!isNumberString(value.millis())) {
        spdlog::warn("{} Could not parse '{:s}'", context, value.millis());
        return badRequest(std::move(context));
      }

      if (value.type().empty() || value.amount().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::param::UNDEFINED || value.amount() == constant::param::UNDEFINED) {
        return badRequest(std::move(context));
      }

      return storage.edit(context.user, std::move(value), id)
             ? context.createResponse(restinio::status_created()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }

  Handler deleteSkull(Context && context, restinio::router::route_params_t && params) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      auto id = restinio::cast_to<std::uint64_t>(params[constant::param::ID]);

      return storage.remove<SkullValue>(context.user, id)
             ? context.createResponse(restinio::status_accepted()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }
}
