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

#include <numeric>
#include <gtest/gtest.h>
#include <boost/any.hpp>
#include <glog/logging.h>

#include "BSONObjBuilder.h"

using namespace bson;

TEST(Basic, EmptyObject) {
  BSONObjBuilder builder;
  BSONObj obj = builder.Done();
  ASSERT_EQ(obj.Dump(), "");
  ASSERT_TRUE(obj.begin() == obj.end());
  ASSERT_EQ(obj.TotalSize(), 5);  // totalSize + EOO
}

static void AppendBSONValue(BSONObjBuilder &builder, Type_t type,
                            const boost::any &data) {
  switch (type) {
    case NumberInt:
      builder.Append("int", *boost::unsafe_any_cast<int>(&data));
      break;
    case NumberLong:
      builder.Append("long", *boost::unsafe_any_cast<long long>(&data));
      break;
    case NumberDouble:
      builder.Append("double", *boost::unsafe_any_cast<double>(&data));
      break;
    case Boolean:
      builder.Append("boolean", *boost::unsafe_any_cast<bool>(&data));
      break;
    case String:
      builder.Append("string", *boost::unsafe_any_cast<Slice>(&data));
      break;
    case Datetime:
      builder.Append("timestamp",
                     *boost::unsafe_any_cast<UnixTimestamp>(&data));
      break;
    default:
      break;
  }
}

TEST(Append, Simple) {
  BSONObjBuilder builder;
  struct S {
    Type_t type;
    boost::any data;
  } a[] = {{NumberInt, std::numeric_limits<int>::max()},
           {NumberInt, std::numeric_limits<int>::min()},
           {NumberLong, std::numeric_limits<long long>::max()},
           {NumberLong, std::numeric_limits<long long>::min()},
           {NumberDouble, std::numeric_limits<double>::max()},
           {NumberDouble, std::numeric_limits<double>::min()},
           {Boolean, true},
           {Boolean, false},
           {String, Slice("abc")},
           {Datetime, UnixTimestamp::Now()}};

  for (const auto &data : a) {
    AppendBSONValue(builder, data.type, data.data);
  }

  BSONObj obj = builder.Done();
  auto i = obj.begin();
  for (const auto &data : a) {
    switch (data.type) {
      case NumberInt: {
        auto v = *boost::unsafe_any_cast<int>(&data.data);
        ASSERT_EQ(v, i->ValueOf<int>());
        break;
      }
      case NumberLong: {
        auto v = *boost::unsafe_any_cast<long long>(&data.data);
        ASSERT_EQ(v, i->ValueOf<long long>());
        break;
      }
      case NumberDouble: {
        auto v = *boost::unsafe_any_cast<double>(&data.data);
        ASSERT_EQ(v, i->ValueOf<double>());
        break;
      }
      case Boolean: {
        auto v = *boost::unsafe_any_cast<bool>(&data.data);
        ASSERT_EQ(v, i->ValueOf<bool>());
        break;
      }
      case String: {
        auto v = *boost::unsafe_any_cast<Slice>(&data.data);
        ASSERT_EQ(v.ToString(), i->ValueOf<Slice>().ToString());
        break;
      }
      case Datetime: {
        auto v = *boost::unsafe_any_cast<UnixTimestamp>(&data.data);
        ASSERT_EQ(v.ToString(), i->ValueOf<UnixTimestamp>().ToString());
        break;
      }
      default:
        DCHECK(IsValidType(data.type)) << ": " << data.type;
        break;
    }
    i++;
  }

  ASSERT_TRUE(i == obj.end());
  ASSERT_EQ(obj.NumFields(), sizeof(a) / sizeof(S));
  LOG(INFO) << obj.Dump();
}