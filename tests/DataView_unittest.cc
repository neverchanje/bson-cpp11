//
// Created by neverchanje on 2/2/16.
//

#include <gtest/gtest.h>

#include "DataView.h"

using namespace bson;

TEST(Basic, WriteNum_Little_Endian) {
  char expect[512], actual[512];

  {
    DataView(actual, LittleEndian::value).WriteNum('a');
    DataView(actual, LittleEndian::value).WriteNum('b', 1);

    expect[0] = 'a';
    expect[1] = 'b';
    ASSERT_TRUE(memcmp(actual, expect, 2) == 0);
  }

  {
    // test platform is little-endian
    int a = -1023;
    unsigned b = 1025;
    DataView(actual, LittleEndian::value).WriteNum(a);
    DataView(actual, LittleEndian::value).WriteNum(b, 4);

    memcpy(expect, &a, 4);
    memcpy(expect + 4, &b, 4);
    ASSERT_TRUE(memcmp(actual, expect, 8) == 0);
  }

  {
    long long a = LLONG_MIN + 1000;
    unsigned long long b = ULLONG_MAX - 1000;
    DataView(actual, LittleEndian::value).WriteNum(a);
    DataView(actual, LittleEndian::value).WriteNum(b, sizeof(a));

    memcpy(expect, &a, sizeof(a));
    memcpy(expect + sizeof(a), &b, sizeof(b));
    ASSERT_TRUE(memcmp(actual, expect, sizeof(a) + sizeof(b)) == 0);
  }

}

