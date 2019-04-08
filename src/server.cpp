#include "server.hpp"

#include <spdlog/spdlog.h>

#include "storage.hpp"

namespace {
  Storage storage;

  struct ServerMode {
    using SingleThread = asio::strand<asio::executor>;
    using MultiThread = asio::executor;
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
}

namespace server {
  void listen(std::string && host, std::uint16_t port, std::uint16_t threadCount) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get(constant::path::QUICK, [](auto request, auto) { return getQuick(request); });
    router->http_post(constant::path::QUICK, [](auto request, auto) { return postQuick(request); });
    router->http_delete(constant::path::QUICK, [](auto request, auto) { return deleteQuick(request); });
    router->http_get(constant::path::SKULL, [](auto request, auto) { return getSkull(request); });
    router->http_post(constant::path::SKULL, [](auto request, auto) { return postSkull(request); });
    router->http_delete(constant::path::SKULL, [](auto request, auto) { return deleteSkull(request); });
    router->non_matched_request_handler([](auto request) { return notFound(request); });

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

  Handler postQuick(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::query::TYPE)
          || !query.has(constant::query::AMOUNT)
          || !query.has(constant::query::ICON)) {
        return badRequest(std::move(context));
      }

      QuickValue value{query[constant::query::TYPE], query[constant::query::AMOUNT], query[constant::query::ICON]};

      if (value.type().empty() || value.amount().empty() || value.icon().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::query::UNDEFINED
          || value.amount() == constant::query::UNDEFINED
          || value.icon() == constant::query::UNDEFINED) {
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

  Handler deleteQuick(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::query::TYPE)
          || !query.has(constant::query::AMOUNT)
          || !query.has(constant::query::ICON)) {
        return badRequest(std::move(context));
      }

      QuickValue value{query[constant::query::TYPE], query[constant::query::AMOUNT], query[constant::query::ICON]};

      if (value.type().empty() || value.amount().empty() || value.icon().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::query::UNDEFINED
          || value.amount() == constant::query::UNDEFINED
          || value.icon() == constant::query::UNDEFINED) {
        return badRequest(std::move(context));
      }

      return storage.remove(context.user, std::move(value))
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

  Handler postSkull(Context && context) noexcept {
    try {
      if (!storage.authorized(context.user)) return forbidden(std::move(context));

      const auto query = restinio::parse_query(context.request->header().query());
      if (!query.has(constant::query::TYPE) || !query.has(constant::query::AMOUNT)) {
        return badRequest(std::move(context));
      }

      SkullValue value{query[constant::query::TYPE],
                       query[constant::query::AMOUNT],
                       std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch()
                       ).count())};

      if (value.type().empty() || value.amount().empty()) {
        return badRequest(std::move(context));
      }

      if (value.type() == constant::query::UNDEFINED || value.amount() == constant::query::UNDEFINED) {
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

      return storage.remove(context.user, std::move(value))
             ? context.createResponse(restinio::status_accepted()).done()
             : context.createResponse(restinio::status_internal_server_error()).done();
    } catch (const std::exception & e) {
      spdlog::error("{} Exception: {:s}", context, e.what());
      return internalServerError(std::move(context));
    }
  }
}
