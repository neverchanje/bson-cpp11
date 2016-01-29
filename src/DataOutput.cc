//
// Created by neverchanje on 9/3/15.
//


#include "Fmt.h"
#include "DataOutput.h"

namespace bson {

/**
 * convertToHex is almost ten times better than snprintf("%p", p);
 */
static char digit_hex[] = "0123456789abcdef";
void DataOutput::formatPtrToString(const void *p) {
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  char buf[32];
  char *cur = buf;
  size_t len;

  *cur++ = '0';
  *cur++ = 'x';
  while (v) {
    *cur++ = digit_hex[v % 16];
    v /= 16;
  }
  *cur = '\0';
  len = cur - buf;
  std::reverse(buf + 2, buf + len);
  appendBuffer(buf, len);
}

template<typename T>
void DataOutput::formatIntToString(T v) {
  static_assert(std::is_integral<T>::value, "v is not a integral value.");
  fmt::FormatInt f(v);
  appendBuffer(f.c_str(), f.size());
}

//TODO: replace it with google/Double-Conversion
template<typename T>
void DataOutput::formatFloatToString(T v) {
  static_assert(std::is_floating_point<T>::value, "v is not a float point value.");
  char buf[32];

  int len = snprintf(buf, sizeof(buf), "%lf", v);
  appendBuffer(buf, static_cast<size_t>(len));
}


DataOutput &DataOutput::WriteUInt(unsigned int v) {
  formatIntToString(v);
  return *this;
}

DataOutput &DataOutput::WriteInt(int v) {
  formatIntToString(v);
  return *this;
}

DataOutput &DataOutput::WriteULong(unsigned long v) {
  formatIntToString(v);
  return *this;
}

DataOutput &DataOutput::WriteLong(long v) {
  formatIntToString(v);
  return *this;
}

DataOutput &DataOutput::WriteLLong(long long v) {
  formatIntToString(v);
  return *this;
}

DataOutput &DataOutput::WriteULLong(unsigned long long v) {
  formatIntToString(v);
  return *this;
}

DataOutput &DataOutput::WriteDouble(double v) {
  formatFloatToString(v);
  return *this;
}

DataOutput &DataOutput::WriteAddress(const void *ptr) {
  formatPtrToString(ptr);
  return *this;
}

} //namespace bson