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
#include <boost/endian/buffers.hpp>

#include "BufBuilder.h"

using namespace bson;
using namespace boost::endian;

TEST(Basic, Append) {
  long long val = 1000LL;
  BufBuilder builder;
  builder.AppendNum(val);
  builder.AppendStr("yes");
  ASSERT_EQ(0, memcmp(builder.Buf(), little_int64_buf_t(val).data(),
                      sizeof(long long)));
  ASSERT_EQ(0, strcmp(builder.Buf() + sizeof(long long), "yes"));

  builder.Clear();
  builder.AppendStr("true");
  builder.AppendNum(val);
  ASSERT_EQ(0, strcmp(builder.Buf(), "true"));
  ASSERT_EQ(0,
            memcmp(builder.Buf() + strlen("true") + 1,  // +1 for trailing '\0'
                   little_int64_buf_t(val).data(), sizeof(long long)));
}