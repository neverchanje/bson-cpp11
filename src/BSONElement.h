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

#include <cstddef>
#include <cstring>
#include <boost/assert.hpp>
#include <folly/Likely.h>
#include <cstdint>

#include "BSONTypes.h"
#include "DataView.h"
#include "BSONObj.h"

namespace bson {

//  BSONElement represents an "element" in a BSONObj.  So for the object
//  { a : 3, b : "abc" }, 'a : 3' is the first element (key+value).
//
//  A BSONElement object points into the BSONObj's data. Thus the BSONObj
//  must stay in scope for the life of the BSONElement.
//
//  internals:
//  <type><fieldName'\0'><value>

class BSONElement {
  static const size_t SZ_Type = 1;
  static const size_t SZ_ValueStrSize = 4;

 public:
  explicit BSONElement(const char *data)
      : fieldNameSize_(-1), elem_(data), totalSize_(0) {}

 public:
  // Total size of the element.
  size_t Size() const;

  BSONTypes Type() const {
    char ret = ConstDataView(elem_).ReadNum<char>();
    return static_cast<BSONTypes>(ret);
  }

  // Field name of the element.  e.g., for
  // name : "Joe"
  // "name" is the fieldname
  const char *FieldName() const {
    if (IsEOO()) {
      return "";
    }
    return elem_ + sizeof(char);
  }

  // @return size of field name including terminating null.
  size_t FieldNameSize() const {
    if (fieldNameSize_ == -1) {
      // +1 for '\0'
      fieldNameSize_ = static_cast<int>(strlen(elem_ + SZ_Type) + 1);
    }
    return static_cast<size_t>(fieldNameSize_);
  }

  // @return raw data of value.
  const char *Value() const {
    return elem_ + sizeof(char) + FieldNameSize();
  }

  // Must assure that the type of this element is BSONTypes::String first.
  // @return String size including terminating null
  size_t ValueStrSize() const {
    return static_cast<size_t>(ConstDataView(Value()).ReadNum<uint32_t>());
  }

  // Must assure that the type of this element is BSONTypes::Object or
  // BSONTypes::Array first.
  // @return embedded object size including the size itself
  size_t ValueObjSize() const {
    return static_cast<size_t>(ConstDataView(Value()).ReadNum<uint32_t>());
  }

  bool IsEOO() const {
    return Type() == BSONTypes::EOO;
  }

 private:
  // Check whether the type of this element equals to "type".
  inline const BSONElement &checkType(BSONTypes type) const {
    BOOST_ASSERT_MSG(type != Type(),
                     "unexpected or missing of type value in BSON object");
    return *this;
  }

 private:
  const char *elem_;

  // cached value
  mutable int fieldNameSize_;
  mutable size_t totalSize_;
};

}  // namespace bson
