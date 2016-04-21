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

#include "BSONElement.h"
#include "Slice.h"
#include "UnixTimestamp.h"

namespace bson {

size_t BSONElement::Size() const {
  size_t valueSize = 0;
  BSONType t = Type();
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
      BOOST_ASSERT(IsValidBSONTypes(t));
  }
  totalSize_ = SZ_Type + valueSize + FieldNameSize();
  return totalSize_;
}

inline const BSONElement &BSONElement::checkType(BSONType type) const {
  DCHECK_EQ(type, Type())
      << "unexpected or missing of type value in BSON object: "
      << BSONTypesToString(Type());
  return *this;
}

template <> int BSONElement::ValueOf<int>() const {
  checkType(NumberInt);
  return *reinterpret_cast<const int *>(RawValue());
}

template <> long long BSONElement::ValueOf<long long>() const {
  checkType(NumberLong);
  return *reinterpret_cast<const long long *>(RawValue());
}

template <> double BSONElement::ValueOf<double>() const {
  checkType(NumberDouble);
  return *reinterpret_cast<const double *>(RawValue());
}

template <> bool BSONElement::ValueOf<bool>() const {
  checkType(Boolean);
  return *reinterpret_cast<const bool *>(RawValue());
}

template <> Slice BSONElement::ValueOf<Slice>() const {
  checkType(String);
  size_t l = ValueStrSize() - 1;
  return Slice(RawValue() + sizeof(int), l);
}

template <> UnixTimestamp BSONElement::ValueOf<UnixTimestamp>() const {
  checkType(Datetime);
  return *reinterpret_cast<const UnixTimestamp *>(RawValue());
}

// template <> const char* BSONElement::ValueOf<const char *>() const {
//  checkType(String);
//  return (RawValue()+ sizeof(int));
//}

}  // namespace bson
