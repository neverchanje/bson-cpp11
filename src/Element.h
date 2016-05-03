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

#include <cstring>
#include <cstdint>
#include <boost/assert.hpp>

#include "Type.h"
#include "DataView.h"

namespace bson {

//  Element represents an "element" in a Object.  So for the object
//  { a : 3, b : "abc" }, 'a : 3' is the first element (key+value).
//
//  A Element object points into the Object's data. Thus the Object
//  must stay in scope for the life of the Element.
//
//  internals:
//  <type><fieldName'\0'><value>
//
class Element {
  static const size_t SZ_Type = 1;
  static const size_t SZ_ValueStrSize = 4;

 public:
  explicit Element(const char *data)
      : fieldNameSize_(-1), elem_(data), totalSize_(0) {}

 public:
  //
  // Observers
  //

  // Total size of the element.
  size_t Size() const;

  Type_t Type() const {
    char ret = ConstDataView(elem_).ReadNum<char>();
    return static_cast<Type_t>(ret);
  }

  // Field name of the element.  e.g., for
  // name : "Joe"
  // "name" is the fieldname
  const char *RawFieldName() const {
    if (Type() == Type_t::kEOO) {
      return "";
    }
    return elem_ + sizeof(char);
  }

  // @return raw data of value.
  const char *RawValue() const {
    return elem_ + sizeof(char) + FieldNameSize();
  }

  const char *RawData() const {
    return elem_;
  }

  // @return size of field name including terminating null.
  size_t FieldNameSize() const {
    if (fieldNameSize_ == -1) {
      // +1 for '\0'
      fieldNameSize_ = static_cast<int>(strlen(elem_ + SZ_Type) + 1);
    }
    return static_cast<size_t>(fieldNameSize_);
  }

  // Must assure that the type of this element is Type_t::kString first.
  // @return String size including terminating null
  size_t ValueStrSize() const {
    return static_cast<size_t>(ConstDataView(RawValue()).ReadNum<int>());
  }

  // Must assure that the type of this element is Type_t::kObject or
  // Type_t::kArray first.
  // @return embedded object size including the size itself
  size_t ValueObjSize() const {
    return static_cast<size_t>(ConstDataView(RawValue()).ReadNum<int>());
  }

  // Stores the value of this Element into val.
  template <class T> T ValueOf() const {
    BOOST_ASSERT_MSG(0, "Unexpected type of value in BSON object.");
    return T();
  }

 private:
  // Check whether the type of this element equals to "type".
  inline const Element &checkType(Type_t type) const;

 private:
  const char *elem_;

  // cached value
  mutable int fieldNameSize_;
  mutable size_t totalSize_;
};

}  // namespace bson
