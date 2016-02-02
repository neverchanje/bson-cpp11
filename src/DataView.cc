//
// Created by neverchanje on 1/31/16.
//

#include <boost/endian/buffers.hpp>

#include "DataView.h"

using namespace boost::endian;

namespace bson {

// Use boost::endian library to converse the endian of integral numbers.
// (boost::endian doesn't provide endian conversion for float point numbers,
// but we can easily implement it.)

#define WRITE_NUM_IMPL(data_type) \
  if(is_le_) { \
    little_##data_type##_buf_t t(static_cast<data_type##_t>(val)); \
    writeBuffer(t.data(), sizeof(data_type##_t), offset); \
  } else { \
    big_##data_type##_buf_t t(static_cast<data_type##_t>(val)); \
    writeBuffer(t.data(), sizeof(data_type##_t), offset); \
  } \
  return (*this)


DataView &DataView::WriteUChar(unsigned char val, size_t offset) {
  WRITE_NUM_IMPL(uint8);
}

DataView &DataView::WriteChar(char val, size_t offset) {
  static_assert(CHAR_BIT == 8, "CHAR_BIT == 8");
  WRITE_NUM_IMPL(int8);
}

DataView &DataView::WriteUShort(unsigned short val, size_t offset) {
  WRITE_NUM_IMPL(uint16);
}

DataView &DataView::WriteShort(short val, size_t offset) {
  static_assert(sizeof(short) == 2, "sizeof(short) == 2");
  WRITE_NUM_IMPL(int16);
}

DataView &DataView::WriteUInt(unsigned val, size_t offset) {
  WRITE_NUM_IMPL(uint32);
}

DataView &DataView::WriteInt(int val, size_t offset) {
  static_assert(sizeof(int) == 4, "sizeof(int) == 4");
  WRITE_NUM_IMPL(int32);
}

DataView &DataView::WriteULLong(unsigned long long val, size_t offset) {
  WRITE_NUM_IMPL(uint64);
}

DataView &DataView::WriteLLong(long long val, size_t offset) {
  static_assert(sizeof(long long) == 8, "sizeof(long long) == 8");
  WRITE_NUM_IMPL(int64);
}

#undef WRITE_NUM_IMPL

DataView &DataView::WriteFloat(float val, size_t offset) {
  static_assert(sizeof(float) == 4, "sizeof(float) == 4");
  uint32_t temp;
  memcpy(&temp, &val, sizeof(val));
  WriteUInt(temp, offset);
  return (*this);
}

DataView &DataView::WriteDouble(double val, size_t offset) {
  static_assert(sizeof(double) == 8, "sizeof(double) == 8");
  uint64_t temp;
  memcpy(&temp, &val, sizeof(val));
  WriteULLong(temp, offset);
  return (*this);
}

} // namespace bson
