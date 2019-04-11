#include <gtest/gtest.h>

#include <sstream>

#include "skull_value.hpp"
#include "quick_value.hpp"
#include "user.hpp"

TEST(SkullValue, json) {
  SkullValue skull{"type", "1", "2"};
  std::stringstream stream;
  skull.json(stream);
  ASSERT_EQ(stream.str(), "{\"type\":\"type\",\"amount\":1,\"millis\":2}");
}

TEST(SkullValue, tsv) {
  SkullValue skull{"type", "1", "2"};
  std::stringstream stream;
  skull.tsv(stream);
  ASSERT_EQ(stream.str(), "type\t1\t2");
}

TEST(QuickValue, json) {
  QuickValue quick{"type", "1", "icon"};
  std::stringstream stream;
  quick.json(stream);
  ASSERT_EQ(stream.str(), "{\"type\":\"type\",\"amount\":1,\"icon\":\"icon\"}");
}

TEST(QuickValue, tsv) {
  QuickValue quick{"type", "1", "icon"};
  std::stringstream stream;
  quick.tsv(stream);
  ASSERT_EQ(stream.str(), "type\t1\ticon");
}

TEST(User, keeps_name_reference) {
  std::unique_ptr<User> user;
  {
    std::string name{"username"};
    user = std::make_unique<User>(name);
  }

  ASSERT_STREQ(user->name, "username");
}

TEST(SkullValue, equals) {
  SkullValue skull1{"type", "1", "2"};
  SkullValue skull2{"type", "1", "2"};
  SkullValue skull3{"type", "1", "3"};

  ASSERT_EQ(skull1, skull2);
  ASSERT_NE(skull1, skull3);
}

TEST(QuickValue, equals) {
  QuickValue quick1{"type", "1", "2"};
  QuickValue quick2{"type", "1", "2"};
  QuickValue quick3{"type", "1", "3"};

  ASSERT_EQ(quick1, quick2);
  ASSERT_NE(quick1, quick3);
}

TEST(User, equals) {
  User user1{"username"};
  User user2{"username"};
  User user3{"username2"};

  ASSERT_EQ(user1, user2);
  ASSERT_NE(user1, user3);
}
