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

class BSONObjBuilder {
  __DISALLOW_COPYING__(BSONObjBuilder);

 public:
  BSONObjBuilder() : doneCalled_(false) {
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

  BSONObjBuilder &AppendNull(Slice field, std::nullptr_t val = nullptr) {
    appendBSONType(BSONType::Null);
    buf_.AppendStr(field);
    return *this;
  }

  BSONObjBuilder &Append(Slice field, std::nullptr_t val = nullptr) {
    return AppendNull(field, val);
  }

  BSONObjBuilder &AppendBool(Slice field, bool val) {
    appendBSONType(BSONType::Boolean);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<char>(val ? 1 : 0));
    return *this;
  }

  BSONObjBuilder &Append(Slice field, bool val) {
    return AppendBool(field, val);
  }

  BSONObjBuilder &AppendDouble(Slice field, double val) {
    appendBSONType(BSONType::NumberDouble);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  BSONObjBuilder &Append(Slice field, double val) {
    return AppendDouble(field, val);
  }

  BSONObjBuilder &AppendLong(Slice field, long long val) {
    appendBSONType(BSONType::NumberLong);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  BSONObjBuilder &Append(Slice field, long long val) {
    return AppendLong(field, val);
  }

  BSONObjBuilder &AppendInt(Slice field, int val) {
    appendBSONType(BSONType::NumberInt);
    buf_.AppendStr(field);
    buf_.AppendNum(val);
    return *this;
  }

  BSONObjBuilder &Append(Slice field, int val) {
    return AppendInt(field, val);
  }

  // Append a string element including NULL terminator.
  BSONObjBuilder &AppendStr(Slice field, Slice str) {
    appendBSONType(BSONType::String);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<int>(str.Len() + 1));
    buf_.AppendStr(str);
    return *this;
  }

  BSONObjBuilder &Append(Slice field, Slice str) {
    return AppendStr(field, str);
  }

  // Add header for a new subobject.
  BSONObjBuilder &AppendSubObjectHeader(Slice field) {
    appendBSONType(BSONType::Object);
    buf_.AppendStr(field);
    return *this;
  }

  // Add header for a new subarray.
  BSONObjBuilder &AppendSubArrayHeader(Slice field) {
    appendBSONType(BSONType::Array);
    buf_.AppendStr(field);
    return *this;
  }

  // Append a embedded object.
  BSONObjBuilder &AppendObject(Slice field, const BSONObj &obj) {
    appendBSONType(BSONType::Object);
    buf_.AppendStr(field);
    buf_.AppendBuf(obj.RawData(), obj.TotalSize());
    return *this;
  }

  BSONObjBuilder &Append(Slice field, const BSONObj &obj) {
    return AppendObject(field, obj);
  }

  BSONObjBuilder &AppendArray(Slice field, const BSONArray &arr) {
    appendBSONType(BSONType::Array);
    buf_.AppendStr(field);
    buf_.AppendBuf(arr.RawData(), arr.TotalSize());
    return *this;
  }

  BSONObjBuilder &Append(Slice field, const BSONArray &arr) {
    return AppendArray(field, arr);
  }

  BSONObjBuilder &AppendDatetime(Slice field, const UnixTimestamp &t) {
    appendBSONType(BSONType::Datetime);
    buf_.AppendStr(field);
    buf_.AppendNum(static_cast<int64_t>(t.MicrosSinceEpoch()));
    return *this;
  }

  BSONObjBuilder &Append(Slice field, const UnixTimestamp &t) {
    return AppendDatetime(field, t);
  }

 public:
  // Finish building.
  // @return BSONObj constructed by this BSONObjBuilder.
  BSONObj Done() {
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
      appendBSONType(BSONType::EOO);
      buf_.ClaimReservedBytes(1);

      // set "totalSize" field of the bson object
      DataView(buf_.Buf()).WriteNum(static_cast<int>(buf_.Len()));
    }
  }

  // @return BSONObj constructed by this BSONObjBuilder.
  BSONObj Obj() const {
    BOOST_ASSERT_MSG(doneCalled_, "Building of this object hasn't done yet.");
    return BSONObj(buf_.Buf());
  }

 public:
  // (DEBUG)
  const BufBuilder &TEST_BufBuilder() const {
    return buf_;
  }

 private:
  inline void appendBSONType(BSONType type) {
    buf_.AppendNum(static_cast<char>(type));
  }

 private:
  BufBuilder buf_;
  bool doneCalled_;
};

}  // namespace bson