//
// Created by neverchanje on 1/31/16.
//

#pragma once

#include <cstddef>

#include "DisallowCopying.h"

namespace bson {

class Slice;

/**
 * The DataView view provides a low-level interface for reading and
 * writing multiple number types in an buffer irrespective of the
 * platform's endianness.
 */
class DataView {
  __DISALLOW_COPYING__(DataView);

 public:

  // A DataView object offers reading and writing operations on an
  // existing buffer.
  // Note: the size of the buffer must be ensured available before
  // the read/write operation.
  DataView(char *buf) :
      buf_(buf) {
  }

  // DataView writes numbers in little endian / network byte order.

  DataView &WriteUChar(unsigned char val, size_t offset = 0);
  DataView &WriteChar(char val, size_t offset = 0);
  DataView &WriteUShort(unsigned short val, size_t offset = 0);
  DataView &WriteShort(short val, size_t offset = 0);
  DataView &WriteUInt(unsigned val, size_t offset = 0);
  DataView &WriteInt(int val, size_t offset = 0);
  DataView &WriteULLong(unsigned long long val, size_t offset = 0);
  DataView &WriteLLong(long long val, size_t offset = 0);
  DataView &WriteFloat(float val, size_t offset = 0);
  DataView &WriteDouble(double val, size_t offset = 0);

  template<typename T>
  DataView &WriteNum(T val, size_t offset = 0);

  // T is only available for POD types.
  template<typename T>
  T Read(size_t offset = 0) const {
    T t;
    Read(&t, offset);
    return t;
  }

  // T is only available for POD types.
  template<typename T>
  void Read(T *t, size_t offset = 0) const {
    char *dest = static_cast<char *>(t);
    memcpy(dest, buf_, sizeof(*t));
  }

 private:

  inline void writeBuffer(const char *data, size_t len, size_t offset) {
    memcpy(buf_ + offset, data, len);
  }

 private:
  char *buf_;
};

// type specialization
template<>
DataView &DataView::WriteNum(unsigned char val, size_t offset) {
  return WriteUChar(val, offset);
}

template<>
DataView &DataView::WriteNum(char val, size_t offset) {
  return WriteChar(val, offset);
}

template<>
DataView &DataView::WriteNum(unsigned short val, size_t offset) {
  return WriteUShort(val, offset);
}

template<>
DataView &DataView::WriteNum(short val, size_t offset) {
  return WriteShort(val, offset);
}

template<>
DataView &DataView::WriteNum(unsigned val, size_t offset) {
  return WriteUInt(val, offset);
}

template<>
DataView &DataView::WriteNum(int val, size_t offset) {
  return WriteInt(val, offset);
}

template<>
DataView &DataView::WriteNum(unsigned long long val, size_t offset) {
  return WriteULLong(val, offset);
}

template<>
DataView &DataView::WriteNum(long long val, size_t offset) {
  return WriteLLong(val, offset);
}

} // namespace bson