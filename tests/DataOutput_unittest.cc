//
// Created by neverchanje on 9/16/15.
//

#include <gtest/gtest.h>

#include "DataOutput.h"
#include "FixedBuffer.h"

using namespace bson;

class TestStream: public DataOutput {

 public:

  static const int OUTPUT_STREAM_BUFFER_SIZE = 4096;
  typedef FixedBuffer<OUTPUT_STREAM_BUFFER_SIZE> Buffer; //TestStream::Buffer

  const Buffer &GetBuffer() {
    return buf_;
  }

  TestStream() = default;

  void ResetBuffer() {
    buf_.Reset();
  }

 private:

  void appendBuffer(const char *s, size_t len) override {
    buf_.Append(s, len);
  }

 private:

  Buffer buf_;

};


TEST(Basic, Integers) {

  TestStream ts;
  const TestStream::Buffer &buf = ts.GetBuffer();

  ts.WriteInt(255);
  ASSERT_EQ(buf.Data(), "255");
  ts.WriteInt(-1);
  ASSERT_EQ(buf.Data(), "255-1");
  ts.WriteInt(0);
  ASSERT_EQ(buf.Data(), "255-10");

  ts.ResetBuffer();
  ts.WriteInt(255).WriteChars("*").WriteInt(-1);
  ASSERT_EQ(buf.Data(), "255*-1"); //possible wrong case: 255*-1t
}

#include <limits.h>
TEST(Limit, Integers) {

  TestStream ts;
  const TestStream::Buffer &buf = ts.GetBuffer();

  ts.WriteInt(INT_MAX);
  ASSERT_EQ(buf.Data(), "2147483647");

  ts.ResetBuffer();
  ts.WriteInt(INT_MIN);
  ASSERT_EQ(buf.Data(), "-2147483648");

  ts.ResetBuffer();
  ts.WriteShort(SHRT_MAX);
  ASSERT_EQ(buf.Data(), "32767");

  ts.ResetBuffer();
  ts.WriteShort(SHRT_MIN);
  ASSERT_EQ(buf.Data(), "-32768");

  ts.ResetBuffer();
  ts.WriteLLong(LLONG_MAX);
  ASSERT_EQ(buf.Data(), "9223372036854775807");

  ts.ResetBuffer();
  ts.WriteLLong(LLONG_MIN);
  ASSERT_EQ(buf.Data(), "-9223372036854775808");

  ts.ResetBuffer();
  ts.WriteULLong(ULLONG_MAX);
  ASSERT_EQ(buf.Data(), "18446744073709551615");

  ts.ResetBuffer();
  ts.WriteUInt(UINT32_MAX);
  ASSERT_EQ(buf.Data(), "4294967295");

  ts.ResetBuffer();
  ts.WriteUShort(UINT16_MAX);
  ASSERT_EQ(buf.Data(), "65535");
}
