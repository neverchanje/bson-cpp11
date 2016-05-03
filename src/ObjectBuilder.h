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

#include "Object.h"
#include "Slice.h"
#include "DisallowCopying.h"
#include "BufBuilder.h"
#include "UnixTimestamp.h"

//
// Foward declaration of UnixTimestamp.
//

namespace silly {
class UnixTimestamp;
}

namespace bson {
using silly::UnixTimestamp;
}

namespace bson {

class ObjectBuilder {
  __DISALLOW_COPYING__(ObjectBuilder);

 public:
  ObjectBuilder() : doneCalled_(false) {
    // Leave room for 4 bytes "totalSize".
    buf_.Skip(sizeof(int));

    // reserve 1 byte for EOO
    buf_.ReserveBytes(1);
  }

  //
  // Each of the following Append*** functions adds a BSON element (a key-value
  // pair) to the end of the buffer.
  //
  // Specification from http://bsonspec.org/spec.html.
  //
  //  document	::=	int32 e_list "\x00"	BSON Document. int32 is the
  //  total
  //  number of bytes comprising the document.
  //
  //  e_list	::=	element e_list
  //              |	""
  //  element	::=	"\x01" e_name double    64-bit binary floating point
  //              |	"\x02" e_name string	UTF-8 string
  //              |	"\x03" e_name document	Embedded document
  //              |	"\x04" e_name document	Array
  //              |	"\x08" e_name "\x00"	Boolean "false"
  //              |	"\x08" e_name "\x01"	Boolean "true"
  //              |	"\x09" e_name int64	    UTC datetime
  //              |	"\x0A" e_name           Null value
  //              |	"\x10" e_name int32	    32-bit integer
  //              |	"\x12" e_name int64	    64-bit integer
  //  e_name	::=	cstring	Key name
  //  string	::=	int32 (byte*) "\x00"
  //  cstring	::=	(byte*) "\x00"
  //

  ObjectBuilder &AppendNull(Slice field, std::nullptr_t val = nullptr) {
    appendBSONType(Type_t::kNull);
    buf_.AppendStr(field);
    return *this;
  }

  ObjectBuilder &Append(Slice field, std::nullptr_t val = nullptr) {
    return AppendNull(field, val);
  }

  ObjectBuilder &AppendBool(Slice field, bool val) {
    appendBSONType(Type_t::kBoolean);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<char>(val ? 1 : 0));
    return *this;
  }

  ObjectBuilder &Append(Slice field, bool val) {
    return AppendBool(field, val);
  }

  ObjectBuilder &AppendDouble(Slice field, double val) {
    appendBSONType(Type_t::kNumberDouble);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  ObjectBuilder &Append(Slice field, double val) {
    return AppendDouble(field, val);
  }

  ObjectBuilder &AppendLong(Slice field, long long val) {
    appendBSONType(Type_t::kNumberLong);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  ObjectBuilder &Append(Slice field, long long val) {
    return AppendLong(field, val);
  }

  ObjectBuilder &AppendInt(Slice field, int val) {
    appendBSONType(Type_t::kNumberInt);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  ObjectBuilder &Append(Slice field, int val) {
    return AppendInt(field, val);
  }

  // Append a string element including NULL terminator.
  ObjectBuilder &AppendStr(Slice field, Slice str) {
    appendBSONType(Type_t::kString);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<int>(str.Len() + 1));
    buf_.AppendStr(str);
    return *this;
  }

  ObjectBuilder &Append(Slice field, Slice str) {
    return AppendStr(field, str);
  }

  // Add header for a new subobject.
  ObjectBuilder &AppendSubObjectHeader(Slice field) {
    appendBSONType(Type_t::kObject);
    buf_.AppendStr(field);
    return *this;
  }

  // Add header for a new subarray.
  ObjectBuilder &AppendSubArrayHeader(Slice field) {
    appendBSONType(Type_t::kArray);
    buf_.AppendStr(field);
    return *this;
  }

  // Append a embedded object.
  ObjectBuilder &AppendObject(Slice field, const Object &obj) {
    appendBSONType(Type_t::kObject);
    buf_.AppendStr(field);
    buf_.AppendBuf(obj.RawData(), obj.TotalSize());
    return *this;
  }

  ObjectBuilder &Append(Slice field, const Object &obj) {
    return AppendObject(field, obj);
  }

  ObjectBuilder &AppendArray(Slice field, const Array &arr) {
    appendBSONType(Type_t::kArray);
    buf_.AppendStr(field);
    buf_.AppendBuf(arr.RawData(), arr.TotalSize());
    return *this;
  }

  ObjectBuilder &Append(Slice field, const Array &arr) {
    return AppendArray(field, arr);
  }

  ObjectBuilder &AppendDatetime(Slice field, const UnixTimestamp &t) {
    appendBSONType(Type_t::kDatetime);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<int64_t>(t.MicrosSinceEpoch()));
    return *this;
  }

  ObjectBuilder &Append(Slice field, const UnixTimestamp &t) {
    return AppendDatetime(field, t);
  }

 public:
  // Finish building.
  // @return Object constructed by this ObjectBuilder.
  Object Done() {
    DoneFast();
    return Obj();  // RVO
  }

  bool HasDone() const {
    return doneCalled_;
  }

  // Finish building.
  void DoneFast() {
    if (!doneCalled_) {
      doneCalled_ = true;
      appendBSONType(Type_t::kEOO);
      buf_.ClaimReservedBytes(1);

      // set "totalSize" field of the bson object
      DataView(buf_.Buf()).WriteNum(static_cast<int>(buf_.Len()));
    }
  }

  // @return Object constructed by this ObjectBuilder.
  Object Obj() {
    BOOST_ASSERT_MSG(doneCalled_, "Building of this object hasn't done yet.");
    if (sbuf_.get() == nullptr) {
      sbuf_.reset(buf_.Release());
    }
    assert(sbuf_.get() != nullptr);
    return Object(sbuf_);
  }

 public:
  // (DEBUG)
  BufBuilder &TEST_BufBuilder() {
    return buf_;
  }

  SharedBuffer &TEST_SharedBuffer() {
    return sbuf_;
  }

 private:
  inline void appendBSONType(Type_t type) {
    buf_.AppendNum(static_cast<char>(type));
  }

 private:
  BufBuilder buf_;
  bool doneCalled_;
  SharedBuffer sbuf_;
};

}  // namespace bson