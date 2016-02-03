//
// Created by neverchanje on 1/29/16.
//

#include <cstdlib>
#include <cstring> // memcpy

#include "BufBuilder.h"
#include "Slice.h"

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

void BufBuilder::ensureCapacity(size_t size) {
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
}

void BufBuilder::AppendStr(const Slice &s, bool appendEndingNull) {
  size_t slen = s.Len() + (appendEndingNull ? 1 : 0);
  ensureCapacity(slen);
  s.CopyTo(buf_ + len_, appendEndingNull);
  len_ += slen;
}

} // namespace bson
