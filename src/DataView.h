//
// Created by neverchanje on 1/31/16.
//

#pragma once

#include <cstddef>

#include "DisallowCopying.h"

namespace bson {

class Slice;

struct LittleEndian {
  static const bool value = true;
};

struct BigEndian {
  static const bool value = false;
};

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
  //
  // A DataView constructor must be specified with an endian type,
  // which is a boolean value, true is for little endian, and false
  // for big endian. We construct it like this:
  //
  // DataView(buf, LittleEndian::value)
  //
  DataView(char *buf, bool endianType) :
      buf_(buf),
      is_le_(endianType) {
  }

  // DataView writes numbers in little endian / network byte order.

  DataView &WriteNum(unsigned char val, size_t offset = 0);
  DataView &WriteNum(char val, size_t offset = 0);
  DataView &WriteNum(unsigned short val, size_t offset = 0);
  DataView &WriteNum(short val, size_t offset = 0);
  DataView &WriteNum(unsigned val, size_t offset = 0);
  DataView &WriteNum(int val, size_t offset = 0);
  DataView &WriteNum(unsigned long long val, size_t offset = 0);
  DataView &WriteNum(long long val, size_t offset = 0);
  DataView &WriteNum(float val, size_t offset = 0);
  DataView &WriteNum(double val, size_t offset = 0);

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
  bool is_le_;
};


} // namespace bson