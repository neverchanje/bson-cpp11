//
// Created by neverchanje on 1/29/16.
//

#include <cstdlib>

#include "BufBuilder.h"

namespace bson {

BufBuilder::BufBuilder(size_t initsize) :
    cap_(initsize),
    len_(0),
    buf_(nullptr) {

  if (initsize) {
    buf_ = (char *) std::malloc(initsize);
    //TODO: out of memory exception handling
  }
}

void BufBuilder::kill() {
  if (buf_) {
    std::free(buf_);
    buf_ = nullptr;
  }
  cap_ = 0;
  len_ = 0;
}

char *BufBuilder::ensureCapacity(size_t size) {
  size_t oldcap = cap_;
  size_t minsize = len_ + size;

  if (minsize > oldcap) {
    // grow capacity
    size_t newcap = (oldcap * 3) / 2 + 1;
    if (newcap < minsize)
      newcap = minsize;
    buf_ = (char *) std::realloc(buf_, newcap);
    //TODO: out of memory exception handling

    cap_ = newcap;
  }
  return buf_ + len_;
}

} // namespace bson
