//
// Created by neverchanje on 1/25/16.
//

#include <gtest/gtest.h>

#include "Status.h"

using namespace bson;

TEST(Basic, OK) {
  Status s = Status::OK();
  ASSERT_EQ(s.IsOK(), true);
  ASSERT_EQ(sizeof(s), sizeof(std::unique_ptr<int>));
  ASSERT_EQ(s.ToString(), "OK");
}

TEST(Basic, FailedToParse) {
  Status s = Status::FailedToParse("test");
  ASSERT_EQ(s.IsFailedToParse(), true);
  ASSERT_EQ(s.ToString(), "FailedToParse: test");
}

TEST(Basic, Copy) {
  Status s = Status::FailedToParse("test");
  ASSERT_EQ(s.IsOK(), false);

  Status s2 = s;
  ASSERT_EQ(s2.IsFailedToParse(), true);
  ASSERT_EQ(s2.ToString(), s.ToString());

  Status s3(s2);
  ASSERT_EQ(s2.IsFailedToParse(), true);
  ASSERT_EQ(s2.ToString(), s.ToString());
}