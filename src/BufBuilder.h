/**
 * Copyright (C) 2016, Wu Tao All rights reserved.
 *
 * bson is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bson is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <type_traits>
#include <boost/assert.hpp>

#include "DataView.h"
#include "Slice.h"
#include "DisallowCopying.h"

namespace bson {

// std::enable_if_t is not available in C++11
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

class BufBuilder {
  __DISALLOW_COPYING__(BufBuilder);

 public:
  BufBuilder(size_t init_size = 512)
      : cap_(init_size), len_(0), buf_(nullptr), reservedBytes_(0) {
    if (init_size) {
      buf_ = (char*)std::malloc(init_size);
      BOOST_ASSERT_MSG(buf_ != nullptr,
                       "out of memory in BufBuilder::BufBuilder");
    }
  }

  ~BufBuilder() {
    kill();
  }

  // Append number to the end of the buffer in endian-less format.
  // @param "v" only accepts integral and float point numbers.
  template <class T, typename = enable_if_t<std::is_integral<T>::value ||
                                            std::is_floating_point<T>::value>>
  void AppendNum(T v) {
    ensureCapacity(sizeof(v));
    DataView(buf_ + len_).WriteNum(v);
    len_ += sizeof(v);
  }

  // By default, AppendStr automatically appends '\0' to the end of the
  // buffer.
  void AppendStr(Slice s, bool appendEndingNull = true) {
    size_t slen = s.Len() + (appendEndingNull ? 1 : 0);
    ensureCapacity(slen);
    s.CopyTo(buf_ + len_, appendEndingNull);
    len_ += slen;
  }

  // Appends "len" bytes from "s" to buffer.
  void AppendBuf(const char* s, size_t len) {
    ensureCapacity(len);
    memcpy(buf_, s, len);
  }

  // Leave room for some stuff later.
  void Skip(size_t n) {
    ensureCapacity(n);
    len_ += n;
  }

  // Reserve room for some number of bytes to be claimed at a later time.
  void ReserveBytes(size_t n) {
    ensureCapacity(n);
    reservedBytes_ += n;
  }

  // Claim an earlier reservation of some number of bytes. These bytes must
  // already have been eserved. Appends of up to this many bytes immediately
  // following a claim are guaranteed to succeed without a need to reallocate.
  void ClaimReservedBytes(size_t n) {
    BOOST_ASSERT(reservedBytes_ >= n);
    reservedBytes_ -= n;
  }

  void Clear() {
    len_ = 0;
    reservedBytes_ = 0;
  }

 public:

  //
  // Observers
  //

  const char* Buf() const {
    return buf_;
  }

  size_t Len() const {
    return len_;
  }

  size_t Cap() const {
    return cap_;
  }

 private:
  // Release the resources and reinitialize the variables.
  void kill();

  // Ensure that the capacity of buffer is large enough for the needed
  // size of memory, if not, then grow the capacity.
  // @param size is the number of bytes needed.
  void ensureCapacity(size_t size);

 private:
  char* buf_;
  size_t cap_;
  size_t len_;
  size_t reservedBytes_;
};

}  // namespace bson4