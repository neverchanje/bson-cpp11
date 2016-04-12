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

namespace bson {

/**
 * The complete list of BSON types can be found on
 * http://bsonspec.org/spec.html.
 * By now only a subset of them are implemented.
 */
enum BSONType {

  // End of object
  EOO = 0,

  // double precision floating point value
  NumberDouble = 1,

  String = 2,

  // an embedded object
  Object = 3,

  // an embedded array
  Array = 4,

  // boolean type
  Boolean = 8,

  Null = 10,

  // 32-bit integers
  NumberInt = 16,

  // 64-bit integers
  NumberLong = 18,
};

extern const char *BSONTypesToString(BSONType t);

//
// Type traits
//

class BSONObj;
class BSONArray;

template <class T> struct is_valid_bson_type : public std::false_type {};
template <> struct is_valid_bson_type<double> : public std::true_type {};
template <> struct is_valid_bson_type<int> : public std::true_type {};
template <> struct is_valid_bson_type<long long> : public std::true_type {};
template <> struct is_valid_bson_type<BSONObj> : public std::true_type {};
template <> struct is_valid_bson_type<BSONArray> : public std::true_type {};
template <> struct is_valid_bson_type<bool> : public std::true_type {};
template <>
struct is_valid_bson_type<std::nullptr_t> : public std::true_type {};

inline bool IsValidBSONTypes(BSONType type) {
  switch (type) {
    case EOO:
    case NumberInt:
    case NumberLong:
    case NumberDouble:
    case Null:
    case Boolean:
    case String:
    case Object:
    case Array:
      return true;
    default:
      return false;
  }
}

}  // namespace bson