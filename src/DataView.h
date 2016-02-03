//
// Created by neverchanje on 1/31/16.
//

#pragma once

#include <cstddef>

#include "DisallowCopying.h"

namespace bson {

class Slice;

enum EndianType {
  kNativeEndian = 0,
  kLittleEndian = 1,
  kBigEndian = 2
};

struct NativeEndian {
  static const EndianType value = EndianType::kNativeEndian;
};

struct LittleEndian {
  static const EndianType value = EndianType::kLittleEndian;
};

struct BigEndian {
  static const EndianType value = EndianType::kBigEndian;
};

/**
 * The DataView view provides a low-level interface for reading and
 * writing multiple number types in an buffer irrespective of the
 * platform's endianness.
 *
 * Endianess independence is useful for portable binary files and
 * network I/O formats.
 */
class DataView {
  __DISALLOW_COPYING__(DataView);

 public:

  // A DataView object offers reading and writing operations on an
  // existing buffer.
  // Note: the size of the buffer must be ensured enough before
  // the read/write operation.
  //
  // A DataView constructor must be specified with an endian type,
  // which is a boolean value, true is for little endian, and false
  // for big endian. We construct it like this:
  //
  // DataView(buf, LittleEndian::value)
  //
  DataView(char *buf, EndianType endianType) :
      buf_(buf),
      endian_(endianType) {
  }

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

  void ReadNum(unsigned char *val, size_t offset = 0) const;
  void ReadNum(char *val, size_t offset = 0) const;
  void ReadNum(unsigned *val, size_t offset = 0) const;
  void ReadNum(int *val, size_t offset = 0) const;
  void ReadNum(unsigned short *val, size_t offset = 0) const;
  void ReadNum(short *val, size_t offset = 0) const;
  void ReadNum(unsigned long long *val, size_t offset = 0) const;
  void ReadNum(long long *val, size_t offset = 0) const;
  void ReadNum(float *val, size_t offset = 0) const;
  void ReadNum(double *val, size_t offset = 0) const;

  template<typename T>
  T ReadNum(size_t offset = 0) const {
    T t;
    ReadNum(&t, offset);
    return t;
  }

 private:

  inline void writeBuffer(const char *data, size_t len, size_t offset) {
    memcpy(buf_ + offset, data, len);
  }

 private:
  char *buf_;
  EndianType endian_;
};

} // namespace bson