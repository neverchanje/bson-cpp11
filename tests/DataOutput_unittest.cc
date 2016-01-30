//
// Created by neverchanje on 9/16/15.
//

#include <gtest/gtest.h>

#include "DataOutput.h"

using namespace bson;

class TestStream: public DataOutput {

 public:

  TestStream() = default;

  void Reset() {
    buf_[0] = '\0';
  }

  Slice Data() const { return buf_; }

 private:

  void appendBuffer(const Slice &s) override {
    // null character has been appended to the end of s.
    memcpy(buf_, s.RawData(), s.Len());
  }

 private:
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

  ts.Reset();
  ts.WriteInt(255).WriteChars("*").WriteInt(-1);
  ASSERT_EQ(ts.Data(), "255*-1"); //possible wrong case: 255*-1t
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
