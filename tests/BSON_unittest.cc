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
#include <glog/logging.h>

#include "BSONParser.h"

using namespace bson;

TEST(Parser, Empty) {
  BSONObjBuilder builder;
  BSONParser parser("{}");

  Status r = parser.Parse(builder);
  if (!r) {
    LOG(ERROR) << r.ToString() << std::endl;
  }

  ASSERT_TRUE(builder.HasDone());
  BSONObj obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 0);
  LOG(INFO) << "TEST Empty: " << obj.Dump() << std::endl;
}

TEST(Parser, Basic) {
  BSONObjBuilder builder;
  BSONParser parser(
      "{"
      "'1' : 2147483647, "
      "'2' : -2147483648, "
      "'3' : -9223372036854775808, "
      "'4' : 9223372036854775807, "
      "'5' : 1.79769e+30, "
      "'6' : 2.22507e-30,"
      "'7' : true,"
      "'8' : false,"
      "'9' : null,"
      "'10' : \"sunshine boys\""
      "}");

  Status r = parser.Parse(builder);
  if (!r) {
    LOG(ERROR) << r.ToString() << std::endl;
  }

  BSONObj obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 10);

  //  LOG(INFO) << "TEST Basic: " << obj.Dump() << std::endl;
}

TEST(Parser, Embedded) {
  BSONObjBuilder builder;
  BSONParser parser(
      "["
      "{"
      "'1' : 2147483647, "
      "'2' : -2147483648 "
      "},"
      "{"
      "'x' : {"
      "'3' : -9223372036854775808, "
      "'4' : 9223372036854775807"
      "},"
      "'y' : {"
      "'5' : 1.79769e+30, "
      "'6' : 2.22507e-30"
      "}"
      "},"
      "{"
      "'7' : true,"
      "'8' : false,"
      "'9' : null,"
      "'10' : \"sunshine boys\""
      "}"
      "]");

  Status r = parser.Parse(builder);
  if (!r) {
    LOG(ERROR) << r.ToString() << std::endl;
  }

  BSONObj obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 3);
  LOG(INFO) << "TEST Embedded: " << obj.Dump() << std::endl;
}

TEST(Parser, Special) {
  BSONObjBuilder builder;
  BSONParser parser(
      "{"
      "'1' : Timestamp(2147483647, 100)"
      "}");

  Status r = parser.Parse(builder);
  if (!r) {
    LOG(ERROR) << r.ToString() << std::endl;
  }

  BSONObj obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 1);
  LOG(INFO) << "TEST Special: " << obj.Dump() << std::endl;
}
