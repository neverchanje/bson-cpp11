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
  BufBuilder(size_t init_size = 512);

  ~BufBuilder() { kill(); }

  template <class T, typename = enable_if_t<std::is_integral<T>::value ||
                                            std::is_floating_point<T>::value>>
  void AppendNum(T v) {
    ensureCapacity(sizeof(v));
    DataView(buf_ + len_, kLittleEndian).WriteNum(v);
    len_ += sizeof(v);
  }

  void AppendStr(Slice s, bool appendEndingNull = true);

  const char* Buf() const { return buf_; }

  void Clear() { len_ = 0; }

 private:
  void kill();

  // Ensure the capacity of buffer is large enough for the needed
  // size of memory.
  // @param size is the number of bytes needed.
  void ensureCapacity(size_t size);

 private:
  char* buf_;
  size_t cap_;
  size_t len_;
};

}  // namespace bson