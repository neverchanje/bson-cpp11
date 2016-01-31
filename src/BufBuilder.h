//
// Created by neverchanje on 1/29/16.
//


#pragma once

#include "DisallowCopying.h"
#include "DataOutput.h"

namespace bson {

class BufBuilder: public DataOutput {
  __DISALLOW_COPYING__(BufBuilder);

 public:

  BufBuilder(size_t initsize = 512);

  ~BufBuilder() { kill(); }

  size_t Len() const { return len_; }

  size_t Capacity() const { return cap_; }

  const char *Data() const { return buf_; }
  char *Data() { return buf_; }

  size_t Avail() const { return cap_ - len_; }

  void Append(const char *s, size_t len) {
    memcpy(ensureCapacity(len), s, len);
  }

 private:

  void kill();

  // Ensure the capacity of buffer is large enough for the needed
  // size of memory.
  // @param size is the number of bytes needed.
  char *ensureCapacity(size_t size);

 private:
  char *buf_;
  size_t cap_;
  size_t len_;
};

} // namespace bson