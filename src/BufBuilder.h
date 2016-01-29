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

  BufBuilder() :
      buf_(nullptr),
      cap_(0),
      len_(0) {
  }

  BufBuilder(size_t initsize = 512) {
    if (initsize) {
      buf_ = (char *) std::malloc(initsize);
      // out of memory exception handling
      cap_ = initsize;
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
    if (len_) {

    }
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

  //
  char *grow(size_t size) {

  }

 private:
  char *buf_;
  size_t cap_;
  size_t len_;
};

} // namespace bson