//
// Created by neverchanje on 9/3/15.
//

#pragma once

#include <string>

namespace bson {

/**
 * FixedBuffer has fixed size, which is given in declaration, and
 * will not be allowed to change the buffer size after creation.
 */
template<size_t BUFSIZE>
class FixedBuffer {

 public:

  FixedBuffer() :
      cur_(buf_) {
    memset(buf_, 0, sizeof(buf_));
  }

  ~FixedBuffer() = default;

  const char *Data() const { return buf_; }
  char *Data() { return buf_; }

  size_t Cap() const { return BUFSIZE; }

  size_t Avail() const { return BUFSIZE - Len(); }

  size_t Len() const { return cur_ - buf_; }

  //Concatenate the contents of s to the end of buffer.
  void Append(const char *s, size_t len) {
    if (len < Avail()) {
      memcpy(cur_, s, len);
      cur_ += len;
      *cur_ = '\0';
    }
  }

  void Reset() {
    cur_ = buf_;
    *cur_ = '\0';
  }

 private:

  char buf_[BUFSIZE];

  char *cur_; //iterator

};

} //namespace bson
