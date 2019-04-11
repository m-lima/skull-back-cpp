#include <gtest/gtest.h>

#include <sstream>
#include <iostream>

#include "server.hpp"

TEST(StreamResponse, size_watcher) {
  std::stringstream stream;

  stream << "this is a string";
  ASSERT_EQ(stream.tellp(), 16);

  stream << "this is another string";
  ASSERT_EQ(stream.tellp(), 16 + 22);

  stream.str(std::string{});
  ASSERT_EQ(stream.tellp(), 0);

  stream << "this is another another string";
  ASSERT_EQ(stream.tellp(), 30);
}
