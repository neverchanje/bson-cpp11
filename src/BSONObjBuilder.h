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

#include "BSONObj.h"
#include "Slice.h"
#include "DisallowCopying.h"
#include "BufBuilder.h"
#include "BSONTypes.h"

namespace bson {

class BSONObjBuilder {
  __DISALLOW_COPYING__(BSONObjBuilder);

 public:
  BSONObjBuilder() : doneCalled_(false) {
    // Leave room for 4 bytes "totalSize".
    buf_.Skip(sizeof(uint32_t));

    // reserve 1 byte for EOO
    buf_.ReserveBytes(1);
  }

  //
  // Each of the following Append*** functions adds a BSON element (a key-value
  // pair) to the end of the buffer.
  // Internally, every element of BSON is in the format of :
  // <type><field><value>
  //

  BSONObjBuilder &AppendNull(Slice field) {
    appendBSONType(BSONTypes::Null);
    buf_.AppendStr(field);
    return *this;
  }

  BSONObjBuilder &AppendBool(Slice field, bool val) {
    appendBSONType(BSONTypes::Boolean);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<char>(val ? 1 : 0));
    return *this;
  }

  BSONObjBuilder &AppendDouble(Slice field, double val) {
    appendBSONType(BSONTypes::NumberDouble);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  BSONObjBuilder &AppendLong(Slice field, long long val) {
    appendBSONType(BSONTypes::NumberLong);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  // Append a string element including NULL terminator.
  BSONObjBuilder &AppendStr(Slice field, Slice str) {
    appendBSONType(BSONTypes::String);
    buf_.AppendStr(field);
    buf_.AppendNum(str.Len() + 1);
    buf_.AppendStr(str);
    return *this;
  }

  BSONObjBuilder &AppendBuf(Slice field, const char *str, size_t len) {
    appendBSONType(BSONTypes::String);
    buf_.AppendStr(field);
    buf_.AppendNum(len);
    buf_.AppendBuf(str, len);
    return *this;
  }

  // Add header for a new subobject.
  BSONObjBuilder &AppendSubObjectHeader(Slice field) {
    appendBSONType(BSONTypes::Object);
    buf_.AppendStr(field);
    return *this;
  }

  // Add header for a new subarray.
  BSONObjBuilder &AppendSubArrayHeader(Slice field) {
    appendBSONType(BSONTypes::Array);
    buf_.AppendStr(field);
    return *this;
  }

  BSONObjBuilder &AppendSubObject(Slice field, const BSONObj &obj) {
    appendBSONType(BSONTypes::Object);
    buf_.AppendStr(field);
    buf_.AppendBuf(obj.RawData(), obj.TotalSize());
    return *this;
  }

 public:
  void Done() {
    if (doneCalled_) {
      return;
    }
    doneCalled_ = true;
    buf_.AppendNum(BSONTypes::EOO);
    buf_.ClaimReservedBytes(1);
  }

  BSONObj Obj() const {
    return obj_;
  };

 private:
  inline void appendBSONType(BSONTypes type) {
    buf_.AppendNum(static_cast<char>(type));
  }

 private:
  BSONObj obj_;
  BufBuilder buf_;
  bool doneCalled_;
};

}  // namespace bson
