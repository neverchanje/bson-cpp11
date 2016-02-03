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
  if (endian_ == EndianType::kLittleEndian) { \
    little_##data_type##_buf_t t(static_cast<data_type##_t>(val)); \
    writeBuffer(t.data(), sizeof(data_type##_t), offset); \
  } else if (endian_ == EndianType::kBigEndian) { \
    big_##data_type##_buf_t t(static_cast<data_type##_t >(val)); \
    writeBuffer(t.data(), sizeof(data_type##_t), offset); \
  } else { \
    char *t = reinterpret_cast<char *>(&val); \
    writeBuffer(t, sizeof(data_type##_t), offset); \
  } \
  return (*this)


DataView &DataView::WriteNum(unsigned char val, size_t offset) {
  WRITE_NUM_IMPL(uint8);
}

DataView &DataView::WriteNum(char val, size_t offset) {
  static_assert(CHAR_BIT == 8, "CHAR_BIT == 8");
  WRITE_NUM_IMPL(int8);
}

DataView &DataView::WriteNum(unsigned short val, size_t offset) {
  WRITE_NUM_IMPL(uint16);
}

DataView &DataView::WriteNum(short val, size_t offset) {
  static_assert(sizeof(short) == 2, "sizeof(short) == 2");
  WRITE_NUM_IMPL(int16);
}

DataView &DataView::WriteNum(unsigned val, size_t offset) {
  WRITE_NUM_IMPL(uint32);
}

DataView &DataView::WriteNum(int val, size_t offset) {
  static_assert(sizeof(int) == 4, "sizeof(int) == 4");
  WRITE_NUM_IMPL(int32);
}

DataView &DataView::WriteNum(unsigned long long val, size_t offset) {
  WRITE_NUM_IMPL(uint64);
}

DataView &DataView::WriteNum(long long val, size_t offset) {
  static_assert(sizeof(long long) == 8, "sizeof(long long) == 8");
  WRITE_NUM_IMPL(int64);
}

#undef WRITE_NUM_IMPL

DataView &DataView::WriteNum(float val, size_t offset) {
  static_assert(sizeof(float) == 4, "sizeof(float) == 4");
  uint32_t temp;
  memcpy(&temp, &val, sizeof(val));
  WriteNum(temp, offset);
  return (*this);
}

DataView &DataView::WriteNum(double val, size_t offset) {
  static_assert(sizeof(double) == 8, "sizeof(double) == 8");
  uint64_t temp;
  memcpy(&temp, &val, sizeof(val));
  WriteNum(temp, offset);
  return (*this);
}

#define READ_NUM_IMPL(data_type) \
  memcpy(val, buf_ + offset, sizeof(data_type##_t)); \
  if (endian_ == EndianType::kLittleEndian) { \
    little_to_native_inplace(*val); \
  } else if (endian_ == EndianType::kBigEndian) { \
    big_to_native_inplace(*val); \
  }


void DataView::ReadNum(unsigned char *val, size_t offset) const {
  READ_NUM_IMPL(uint8);
}

void DataView::ReadNum(char *val, size_t offset) const {
  READ_NUM_IMPL(int8);
}

void DataView::ReadNum(unsigned *val, size_t offset) const {
  READ_NUM_IMPL(uint32);
}

void DataView::ReadNum(int *val, size_t offset) const {
  READ_NUM_IMPL(int32);
}

void DataView::ReadNum(unsigned short *val, size_t offset) const {
  READ_NUM_IMPL(uint16);
}

void DataView::ReadNum(short *val, size_t offset) const {
  READ_NUM_IMPL(int16);
}

void DataView::ReadNum(unsigned long long *val, size_t offset) const {
  READ_NUM_IMPL(uint64);
}

void DataView::ReadNum(long long *val, size_t offset) const {
  READ_NUM_IMPL(int64);
}

void DataView::ReadNum(float *val, size_t offset) const {
  uint32_t temp;
  ReadNum(&temp, offset);
  memcpy(val, &temp, sizeof(float));
}

void DataView::ReadNum(double *val, size_t offset) const {
  uint64_t temp;
  ReadNum(&temp, offset);
  memcpy(val, &temp, sizeof(double));
}

} // namespace bson
