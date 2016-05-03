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

#include <cstdlib>
#include <cstring>  // memcpy
#include <boost/assert.hpp>
#include <iostream>

#include "BufBuilder.h"

namespace bson {

void BufBuilder::kill() {
  if (buf_) {
    std::free(buf_);
    buf_ = nullptr;
  }
}

void BufBuilder::ensureCapacity(size_t n) {
  size_t oldcap = cap_;
  size_t minsize = len_ + n + reservedBytes_;

  if (minsize > oldcap) {
    // grow capacity
    size_t newcap = (oldcap * 3) / 2 + 1;
    if (newcap < minsize)
      newcap = minsize;
    buf_ = (char *)std::realloc(buf_, newcap);
    BOOST_ASSERT_MSG(buf_ != nullptr,
                     "out of memory BufBuilder::ensureCapacity");
    cap_ = newcap;
  }
}

}  // namespace bson