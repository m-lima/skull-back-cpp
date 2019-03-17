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
    return std::make_shared<restinio::request_t>(1234, header, std::string{}, connection, restinio::endpoint_t{});
  }
}

//TEST(Authorization, missing_header) {
//  auto request = buildRequest("foo", "bar");
//  ASSERT_FALSE(server::authorize(request).has_value());
//}
//
//TEST(Authorization, correct_capitalization) {
//  auto request = buildRequest("X-User", "test@user.com");
//  ASSERT_TRUE(server::authorize(request).has_value());
//}
//
//TEST(Authorization, lower_case) {
//  auto request = buildRequest("x-user", "test@user.com");
//  ASSERT_TRUE(server::authorize(request).has_value());
//}
//
//TEST(Authorization, upper_case) {
//  auto request = buildRequest("X-USER", "test@user.com");
//  ASSERT_TRUE(server::authorize(request).has_value());
//}
