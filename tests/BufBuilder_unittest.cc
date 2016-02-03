//
// Created by neverchanje on 1/31/16.
//

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
  ASSERT_EQ(0, memcmp(builder.Buf(), little_int64_buf_t(val).data(), sizeof(long long)));
  ASSERT_EQ(0, strcmp(builder.Buf() + sizeof(long long), "yes"));

  builder.Clear();
  builder.AppendStr("true");
  builder.AppendNum(val);
  ASSERT_EQ(0, strcmp(builder.Buf(), "true"));
  ASSERT_EQ(0, memcmp(builder.Buf() + strlen("true") + 1, // +1 for trailing '\0'
                      little_int64_buf_t(val).data(), sizeof(long long)));
}