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
#include <fstream>

#include "Parser.h"
#include "BSON.h"

using namespace bson;

TEST(Parser, Empty) {
  ObjectBuilder builder;
  Parser parser("{}");

  Status r = parser.Parse(builder);
  if (!r) {
    LOG(ERROR) << r.ToString() << std::endl;
  }

  ASSERT_TRUE(builder.HasDone());
  Object obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 0);
  ASSERT_TRUE(obj.begin() == obj.end());

  LOG(INFO) << "TEST Empty: " << obj.Dump() << std::endl;
}

TEST(Parser, Basic) {
  ObjectBuilder builder;
  Parser parser(
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

  Object obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 10);
  LOG(INFO) << "TEST Basic: " << obj.Dump() << std::endl;
}

TEST(Parser, Embedded) {
  ObjectBuilder builder;
  Parser parser(
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

  Object obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 3);
  LOG(INFO) << "TEST Embedded: " << obj.Dump() << std::endl;
}

TEST(Parser, Special) {
  ObjectBuilder builder;
  Parser parser(
      "{"
      "'1' : Datetime(9223372036854775807), "
      "'2' : NumberInt(2147483647), "
      "'3' : NumberLong(-9223372036854775808)"
      "}");

  Status r = parser.Parse(builder);
  if (!r) {
    LOG(ERROR) << r.ToString() << std::endl;
  }

  Object obj = builder.Obj();
  ASSERT_EQ(obj.NumFields(), 3);
  LOG(INFO) << "TEST Special: " << obj.Dump() << std::endl;
}

TEST(Parser, TypeJson) {
  typedef std::istreambuf_iterator<char> iterator_t;

  std::ifstream ifs("../../data/type.json");
  std::string json(iterator_t(ifs), (iterator_t()));

  Object obj = FromJSON(json);

  ASSERT_TRUE(obj.begin() != obj.end());
  LOG(INFO) << obj.Dump();
}