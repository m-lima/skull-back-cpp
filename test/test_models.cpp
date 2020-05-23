#include <gtest/gtest.h>

#include "model.hpp"
#include "user.hpp"

TEST(Skull, json) {
  Skull skull{"1", "nome", "cor", "icone", "2"};
  std::stringstream stream;
  skull.json(stream);
  ASSERT_EQ(stream.str(), R"({"id":1,"name":"nome","color":"cor","icon":"icone","unitPrice":2})");
}

TEST(Skull, tsv) {
  Skull skull{"1", "nome", "cor", "icone", "2"};
  std::stringstream stream;
  skull.tsv(stream);
  ASSERT_EQ(stream.str(), "1\nome\tcor\ticone\t2");
}

TEST(Quick, json) {
  Quick quick{"1", "2"};
  std::stringstream stream;
  quick.json(stream);
  ASSERT_EQ(stream.str(), R"({"skull":1,"amount":2})");
}

TEST(Quick, tsv) {
  Quick quick{"1", "2"};
  std::stringstream stream;
  quick.tsv(stream);
  ASSERT_EQ(stream.str(), "1\t2");
}

TEST(Occurrence, json) {
  Occurrence occurrence{"1", "2", "3", "4"};
  std::stringstream stream;
  occurrence.json(stream);
  ASSERT_EQ(stream.str(), R"({"id":1,"skull":2,"amount":3,"millis":4})");
}

TEST(Occurrence, tsv) {
  Occurrence occurrence{"1", "2", "3", "4"};
  std::stringstream stream;
  occurrence.json(stream);
  ASSERT_EQ(stream.str(), "1\t2\t3\t4");
}

TEST(User, keeps_name_reference) {
  std::unique_ptr<User> user;
  {
    std::string name{"username"};
    user = std::make_unique<User>(name);
  }

  ASSERT_STREQ(user->name, "username");
}

TEST(Skull, equals) {
  Skull skull{"1", "nome", "farge", "ikon", "3"};
  Skull skulls[] = {
      {"1", "nome", "cor", "icone", "2"},
      {"2", "nome", "cor", "icone", "2"},
      {"1", "navn", "cor", "icone", "2"},
      {"2", "navn", "cor", "icone", "2"}
  };

  ASSERT_EQ(skulls[0], skull);

  for (int i = 0; i < 3; ++i) {
    for (int j = i+1; j < 4; ++j) {
      ASSERT_NE(skulls[i], skulls[j]);
    }
  }
}

TEST(Quick, equals) {
  Quick quick{"1", "2"};
  Quick quicks[] = {
      {"1", "2"},
      {"2", "2"},
      {"1", "3"},
      {"2", "3"}
  };

  ASSERT_EQ(quicks[0], quick);

  for (int i = 0; i < 3; ++i) {
    for (int j = i+1; j < 4; ++j) {
      ASSERT_NE(quicks[i], quicks[j]);
    }
  }
}

TEST(Occurrence, equals) {
  Occurrence occurrence{"1","2","3","4"};
  Occurrence same{"1","5","6","7"};
  Occurrence different{"5","2","3","4"};

  ASSERT_EQ(occurrence, same);
  ASSERT_NE(occurrence, different);
}

TEST(User, equals) {
  User user1{"username"};
  User user2{"username"};
  User user3{"username2"};

  ASSERT_EQ(user1, user2);
  ASSERT_NE(user1, user3);
}
