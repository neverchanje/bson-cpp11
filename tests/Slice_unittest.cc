//
// Created by neverchanje on 1/24/16.
//

#include <gtest/gtest.h>

#include "Slice.h"

using namespace bson;

TEST(Basic, Literal) {
  Slice s("abc");
  ASSERT_EQ(s.Empty(), false);
  ASSERT_EQ(s.Len(), 3);
  ASSERT_EQ(s[3], '\0');
}

TEST(Basic, StdString) {
  std::string str("abc");
  Slice s(str);
  ASSERT_EQ(s.Len(), 3);
  ASSERT_EQ(s[3], '\0');
  ASSERT_EQ(*(s.RawData() + 3), '\0');
}

TEST(Basic, Empty) {
  Slice s("");
  ASSERT_EQ(s.Empty(), true);
  ASSERT_EQ(s.Len(), 0);
  ASSERT_NE(s.RawData(), nullptr);
  ASSERT_EQ(s[0], '\0');
  ASSERT_EQ(*s.RawData(), '\0');
}

TEST(Basic, Null) {
  Slice s(nullptr);
  ASSERT_EQ(s.Empty(), true);
  ASSERT_EQ(s.Len(), 0);
  ASSERT_EQ(s.RawData(), nullptr);
}

TEST(Basic, Copy) {
  Slice s("abc");

  char actual[4];
  s.CopyTo(actual);
  ASSERT_TRUE(memcmp(actual, "abc", sizeof(actual)) == 0);
}