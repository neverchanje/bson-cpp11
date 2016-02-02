//
// Created by neverchanje on 9/16/15.
//

#include <gtest/gtest.h>

#include "DataOutput.h"

using namespace bson;

class TestStream: public DataOutput {

 public:

  TestStream() :
      cur_(buf_) {
  }

  void Reset() {
    (*buf_) = '\0';
    cur_ = buf_;
  }

  Slice Data() const { return buf_; }

 private:

  void appendBuffer(const char *s, size_t len) override {
    memcpy(cur_, s, len);
    cur_ += len;
    *cur_ = '\0';
  }

 private:
  char *cur_;
  char buf_[4096];
};

TEST(Basic, Integers) {

  TestStream ts;

  ts.WriteInt(255);
  ASSERT_EQ(ts.Data(), "255");

  ts.WriteInt(-1);
  ASSERT_EQ(ts.Data(), "255-1");

  ts.WriteInt(0);
  ASSERT_EQ(ts.Data(), "255-10");
}

TEST(Basic, Chars) {

  TestStream ts;

  ts.WriteChars("abc");
  ASSERT_EQ("abc", ts.Data());

  ts.WriteInt(-1).WriteChars("*");
  ASSERT_EQ("abc-1*", ts.Data());
}

#include <limits.h>
TEST(Limit, Integers) {

  TestStream ts;

  ts.WriteInt(INT_MAX);
  ASSERT_EQ(ts.Data(), "2147483647");

  ts.Reset();
  ts.WriteInt(INT_MIN);
  ASSERT_EQ(ts.Data(), "-2147483648");

  ts.Reset();
  ts.WriteShort(SHRT_MAX);
  ASSERT_EQ(ts.Data(), "32767");

  ts.Reset();
  ts.WriteShort(SHRT_MIN);
  ASSERT_EQ(ts.Data(), "-32768");

  ts.Reset();
  ts.WriteLLong(LLONG_MAX);
  ASSERT_EQ(ts.Data(), "9223372036854775807");

  ts.Reset();
  ts.WriteLLong(LLONG_MIN);
  ASSERT_EQ(ts.Data(), "-9223372036854775808");

  ts.Reset();
  ts.WriteULLong(ULLONG_MAX);
  ASSERT_EQ(ts.Data(), "18446744073709551615");

  ts.Reset();
  ts.WriteUInt(UINT32_MAX);
  ASSERT_EQ(ts.Data(), "4294967295");

  ts.Reset();
  ts.WriteUShort(UINT16_MAX);
  ASSERT_EQ(ts.Data(), "65535");
}
