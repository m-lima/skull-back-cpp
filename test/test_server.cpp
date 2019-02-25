#include <gtest/gtest.h>

#include "server.hpp"

namespace {
  class SimpleConnection : public restinio::impl::connection_base_t {
  public:
    SimpleConnection() : restinio::impl::connection_base_t(4321L) {}
    ~SimpleConnection() override = default;

    void write_response_parts(restinio::request_id_t,
                              restinio::response_output_flags_t,
                              restinio::write_group_t) override {}
    void check_timeout(std::shared_ptr<restinio::tcp_connection_ctx_base_t> &) override {}
  };

  restinio::request_handle_t buildRequest(restinio::string_view_t field, restinio::string_view_t value) {
    restinio::http_request_header_t header{};
    header.append_field(field, value);
    auto connection = std::make_shared<SimpleConnection>();
    return std::make_shared<restinio::request_t>(1234, header, std::string{}, connection);
  }
}

TEST(Authorization, missing_header) {
  auto request = buildRequest("foo", "bar");
  ASSERT_FALSE(server::authorized(request));
}

TEST(Authorization, single_cookie_with_valid_session) {
  auto request = buildRequest("Cookie", "session=foobar");
  ASSERT_TRUE(server::authorized(request));
}

TEST(Authorization, multiple_cookies_with_valid_session) {
  auto request = buildRequest("Cookie", "first=second;session=foobar;other=yoman");
  ASSERT_TRUE(server::authorized(request));
}

TEST(Authorization, single_cookie_with_invalid_session) {
  auto request = buildRequest("Cookie", "session=");
  ASSERT_FALSE(server::authorized(request));
}

TEST(Authorization, multiple_cookies_with_invalid_session) {
  auto request = buildRequest("Cookie", "first=second;session=;other=yoman");
  ASSERT_FALSE(server::authorized(request));
}

TEST(Authorization, single_cookie_with_missing_session) {
  auto request = buildRequest("Cookie", "first=second");
  ASSERT_FALSE(server::authorized(request));
}

TEST(Authorization, multiple_cookies_with_missing_session) {
  auto request = buildRequest("Cookie", "first=second;other=yoman");
  ASSERT_FALSE(server::authorized(request));
}

TEST(Authorization, single_cookie_with_empty_session) {
  auto request = buildRequest("Cookie", "session=");
  ASSERT_FALSE(server::authorized(request));
}

TEST(Authorization, multiple_cookies_with_empty_session) {
  auto request = buildRequest("Cookie", "first=second;session=;other=yoman");
  ASSERT_FALSE(server::authorized(request));
}
