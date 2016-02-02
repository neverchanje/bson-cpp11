//
// Created by neverchanje on 1/29/16.
//


#pragma once

#include <type_traits>

#include "DisallowCopying.h"
#include "DataView.h"

namespace bson {

class Slice;

// std::enable_if_t is not available in C++11
template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

class BufBuilder {
  __DISALLOW_COPYING__(BufBuilder);

 public:

  BufBuilder(size_t initsize = 512);

  ~BufBuilder() { kill(); }

  template<class T, typename = enable_if_t<std::is_integral<T>::value>>
  void AppendIntegral(T v) {
    DataView(buf_ + len_).WriteNum(v);
  }

  void AppendStr(const Slice &s);

 private:

  void kill();

  // Ensure the capacity of buffer is large enough for the needed
  // size of memory.
  // @param size is the number of bytes needed.
  void ensureCapacity(size_t size);

 private:
  char *buf_;
  size_t cap_;
  size_t len_;
};

} // namespace bson