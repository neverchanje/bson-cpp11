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

#include <string>
#include <glog/logging.h>

#include "Element.h"
#include "Slice.h"
#include "UnixTimestamp.h"

namespace bson {

size_t Element::Size() const {
  size_t valueSize = 0;
  Type_t t = Type();
  switch (t) {
    case EOO:
    case Null:
      break;
    case Boolean:
      valueSize = 1;
      break;
    case NumberInt:
      valueSize = 4;
      break;
    case NumberDouble:
    case NumberLong:
    case Datetime:
      valueSize = 8;
      break;
    case String:
      valueSize = ValueStrSize() + SZ_ValueStrSize;
      break;
    case Object:
    case Array:
      valueSize = ValueObjSize();
      break;
    default:
      BOOST_ASSERT(IsValidType(t));
  }
  totalSize_ = SZ_Type + valueSize + FieldNameSize();
  return totalSize_;
}

inline const Element &Element::checkType(Type_t type) const {
  DCHECK_EQ(type, Type())
      << "unexpected or missing of type value in BSON object: "
      << TypeToString(Type());
  return *this;
}

template <> int Element::ValueOf<int>() const {
  checkType(NumberInt);
  return *reinterpret_cast<const int *>(RawValue());
}

template <> long long Element::ValueOf<long long>() const {
  checkType(NumberLong);
  return *reinterpret_cast<const long long *>(RawValue());
}

template <> double Element::ValueOf<double>() const {
  checkType(NumberDouble);
  return *reinterpret_cast<const double *>(RawValue());
}

template <> bool Element::ValueOf<bool>() const {
  checkType(Boolean);
  return *reinterpret_cast<const bool *>(RawValue());
}

template <> Slice Element::ValueOf<Slice>() const {
  checkType(String);
  size_t l = ValueStrSize() - 1;
  return Slice(RawValue() + sizeof(int), l);
}

template <> UnixTimestamp Element::ValueOf<UnixTimestamp>() const {
  checkType(Datetime);
  return *reinterpret_cast<const UnixTimestamp *>(RawValue());
}

// template <> const char* Element::ValueOf<const char *>() const {
//  checkType(String);
//  return (RawValue()+ sizeof(int));
//}

}  // namespace bson
