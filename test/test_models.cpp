#include <gtest/gtest.h>

#include "model.hpp"
#include "user.hpp"

TEST(Skull, json) {
  Skull skull{1, "nome", "cor", "icone", 2};
  std::stringstream stream;
  skull.json(stream);
  ASSERT_EQ(stream.str(), R"({"id":1,"name":"nome","color":"cor","icon":"icone","unitPrice":2})");
}

TEST(Skull, tsv) {
  Skull skull{1, "nome", "cor", "icone", 2};
  std::stringstream stream;
  skull.tsv(stream);
  ASSERT_EQ(stream.str(), "1\tnome\tcor\ticone\t2");
}

TEST(Quick, json) {
  Quick quick{1, 2};
  std::stringstream stream;
  quick.json(stream);
  ASSERT_EQ(stream.str(), R"({"skull":1,"amount":2})");
}

TEST(Quick, tsv) {
  Quick quick{1, 2};
  std::stringstream stream;
  quick.tsv(stream);
  ASSERT_EQ(stream.str(), "1\t2");
}

TEST(Occurrence, json) {
  Occurrence occurrence{1, 2, 3.0, 4};
  std::stringstream stream;
  occurrence.json(stream);
  ASSERT_EQ(stream.str(), R"({"id":1,"skull":2,"amount":3,"millis":4})");
}

TEST(Occurrence, tsv) {
  Occurrence occurrence{1, 2, 3.2, 4};
  std::stringstream stream;
  occurrence.tsv(stream);
  ASSERT_EQ(stream.str(), "1\t2\t3.2\t4");
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
  Skull reference{1, "nome", "cor", "icone", 3};
  Skull same{1, "navn", "farge", "ikon", 3.0};
  Skull different{2, "nome", "cor", "icone", 3};

  ASSERT_EQ(reference, same);
  ASSERT_NE(reference, different);
}

TEST(Quick, equals) {
  Quick quick{1, 2.1};
  Quick quicks[] = {
      {1, 2.1},
      {2, 2.1},
      {1, 2},
      {2, 2}
  };

  ASSERT_EQ(quicks[0], quick);

  for (int i = 0; i < 3; ++i) {
    for (int j = i+1; j < 4; ++j) {
      ASSERT_NE(quicks[i], quicks[j]);
    }
  }
}

TEST(Occurrence, equals) {
  Occurrence reference{1, 2, 3, 4};
  Occurrence same{1,5,6,7};
  Occurrence different{5,2,3,4};

  ASSERT_EQ(reference, same);
  ASSERT_NE(reference, different);
}

TEST(User, equals) {
  User user1{"username"};
  User user2{"username"};
  User user3{"username2"};

  ASSERT_EQ(user1, user2);
  ASSERT_NE(user1, user3);
}
