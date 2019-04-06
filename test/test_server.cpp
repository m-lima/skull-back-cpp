#include <gtest/gtest.h>

#include <sstream>
#include <iostream>

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
//  std::string json{"[{\"type\":\"mst\",\"amount\":1,\"millis\":1553991448013},{\"type\":\"sns\",\"amount\":1,\"millis\":1554057330541},{\"type\":\"bepo\",\"amount\":18.5,\"millis\":1554057332719}]"};
//  std::stringstream stream{json};
//  std::string buffer;
//
//  // NOT FOUND
//  std::string target{"\"type\":\"sns\",\"amount\":1,\"millis\":1554057330542"};
//
//  // FIRST
////  std::string target{"\"type\":\"mst\",\"amount\":1,\"millis\":1553991448013"};
//
//  // MIDDLE
////  std::string target{"\"type\":\"sns\",\"amount\":1,\"millis\":1554057330541"};
//
//  // LAST
////  std::string target{"\"type\":\"bepo\",\"amount\":18.5,\"millis\":1554057332719"};
//
//  std::cout << "Target: " << target << std::endl;
//  while (std::getline(stream, buffer, '}')) {
//    auto index = buffer.find('{');
//    if (index == buffer.npos) {
//      std::cout << buffer;
//      continue;
//    }
//    auto fraction = buffer.substr(index + 1);
//    if (fraction == target) {
//      if (buffer[0] == '[') {
//        std::cout << '[';
//      }
//      continue;
//    }
//    std::cout << buffer << '}';
//  }
//
//}

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
