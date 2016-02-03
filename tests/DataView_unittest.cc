//
// Created by neverchanje on 2/2/16.
//

#include <gtest/gtest.h>

#include "DataView.h"

using namespace bson;

TEST(Basic, BigEndian) {
  char buf[128];

  int64_t n1 = LLONG_MAX - 1000;
  DataView(buf, BigEndian::value).WriteNum(n1);
  int32_t n2 = INT_MAX - 1000;
  DataView(buf, BigEndian::value).WriteNum(n2, sizeof(n1));
  int16_t n3 = SHRT_MAX - 100;
  DataView(buf, BigEndian::value).WriteNum(n3, sizeof(n1) + sizeof(n2));

  ASSERT_EQ(n1, DataView(buf, BigEndian::value).ReadNum<int64_t>());
  ASSERT_EQ(n2, DataView(buf, BigEndian::value).ReadNum<int32_t>(sizeof(n1)));
  ASSERT_EQ(n3, DataView(buf, BigEndian::value).ReadNum<int16_t>(sizeof(n1) + sizeof(n2)));
}

TEST(Basic, LittleEndian) {
  char buf[128];

  uint64_t n1 = ULLONG_MAX - 1000;
  DataView(buf, LittleEndian::value).WriteNum(n1);
  uint32_t n2 = UINT_MAX - 1000;
  DataView(buf, LittleEndian::value).WriteNum(n2, sizeof(n1));
  uint16_t n3 = USHRT_MAX - 100;
  DataView(buf, LittleEndian::value).WriteNum(n3, sizeof(n1) + sizeof(n2));

  ASSERT_EQ(n1, DataView(buf, LittleEndian::value).ReadNum<uint64_t>());
  ASSERT_EQ(n2, DataView(buf, LittleEndian::value).ReadNum<uint32_t>(sizeof(n1)));
  ASSERT_EQ(n3, DataView(buf, LittleEndian::value).ReadNum<uint16_t>(sizeof(n1) + sizeof(n2)));
}


TEST(Basic, Native_Big_Little) {
  char buf[sizeof(uint32_t) * 3];
  uint32_t native = 1234;

  DataView dv1(buf, NativeEndian::value);
  dv1.WriteNum(native);

  DataView dv2(buf + sizeof(uint32_t), LittleEndian::value);
  dv2.WriteNum(native);

  DataView dv3(buf + sizeof(uint32_t) * 2, BigEndian::value);
  dv3.WriteNum(native);

  ASSERT_EQ(native, dv1.ReadNum<uint32_t>());
  ASSERT_EQ(native, dv2.ReadNum<uint32_t>());
  ASSERT_EQ(native, dv3.ReadNum<uint32_t>());
}
