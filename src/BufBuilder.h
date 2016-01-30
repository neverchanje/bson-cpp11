//
// Created by neverchanje on 1/29/16.
//


#pragma once

#include <cstdlib>

#include "DisallowCopying.h"
#include "DataOutput.h"

namespace bson {

class BufBuilder: public DataOutput {
  __DISALLOW_COPYING__(BufBuilder);

 public:

  BufBuilder(size_t initsize = 512) :
      cap_(initsize),
      len_(0),
      buf_(nullptr) {

    if (initsize) {
      buf_ = (char *) std::malloc(initsize);
      //TODO: out of memory exception handling
    }
  }

  ~BufBuilder() {
    kill();
  }

  size_t Len() const { return len_; }

  size_t Capacity() const { return cap_; }

  const char *Data() const { return buf_; }
  char *Data() { return buf_; }

  size_t Avail() const { return cap_ - len_; }

  void Append(const char *s, size_t len) {
    char *dest = ensureCapacity(len);
    memcpy(dest, s, len);
  }

 private:

  void kill() {
    if (buf_) {
      std::free(buf_);
      buf_ = nullptr;
    }
    cap_ = 0;
    len_ = 0;
  }

  //@param size is the number of bytes needed.
  char *ensureCapacity(size_t size) {
    size_t oldcap = cap_;
    size_t minsize = len_ + size;

    if (minsize > oldcap) {
      // grow capacity
      size_t newcap = (oldcap * 3) / 2 + 1;
      if (newcap < minsize)
        newcap = minsize;
      buf_ = (char *) std::realloc(buf_, newcap);
      //TODO: out of memory exception handling
    }
    return buf_;
  }

 private:
  char *buf_;
  size_t cap_;
  size_t len_;
};

} // namespace bson