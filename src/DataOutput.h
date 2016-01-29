//
// Created by neverchanje on 9/3/15.
//

#pragma once

#include "DisallowCopying.h"
#include "Slice.h"

namespace bson {

/**
 * DataOutput is an interface for formatting basic types of value
 * into string. It's designed to be an alternative of the formatting
 * function in std::ostream.
 *
 * NOTE: DataOutput doesn't own a stream itself, so the subclasses of
 * DataOutput are required to implement the function `appendBuffer`.
 */
class DataOutput {
  __DISALLOW_COPYING__(DataOutput);

 public:

  DataOutput() = default;

  virtual ~DataOutput() = default;

  /**
   * Raw string will be implicitly promoted to boolean rather than
   * std::string, if we don't override operator<< with 'const char *'.
   */
  DataOutput &WriteChars(const Slice s) {
    return *this;
  }

  DataOutput &WriteChar(char v) {
    return WriteInt(static_cast<int>(v));
  }

  DataOutput &WriteUShort(unsigned short v) {
    return WriteUInt(static_cast<unsigned int>(v));
  }

  DataOutput &WriteShort(short v) {
    return WriteInt(static_cast<int>(v));
  }

  DataOutput &WriteUInt(unsigned int v);
  DataOutput &WriteInt(int v);
  DataOutput &WriteULong(unsigned long v);
  DataOutput &WriteLong(long v);
  DataOutput &WriteLLong(long long v);
  DataOutput &WriteULLong(unsigned long long v);

  DataOutput &WriteBool(bool v) {
    return WriteChars(v ? "true" : "false");
  }

  DataOutput &WriteDouble(double v);

  DataOutput &WriteFloat(float v) {
    return WriteDouble(static_cast<double>(v));
  }

  // Print the address of the pointer in hexadecimal.
  DataOutput &WriteAddress(const void *ptr);

 private:

  template<typename T>
  void formatIntToString(T v);

  template<typename T>
  void formatFloatToString(T v);

  void formatPtrToString(const void *ptr);

 protected:

  /**
   * DataOutput reserve the implementation of how you append
   * string to the buffer.
   * `appendBuffer` is used and only used in operator<<.
   */
  virtual void appendBuffer(const char *s, size_t len) = 0;

  virtual void appendBuffer(const Slice &s) = 0;
};

} //namespace bson
