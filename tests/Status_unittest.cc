/**
 * Copyright (C) 2016, Wu Tao All rights reserved.
 *
 * bson is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bson is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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