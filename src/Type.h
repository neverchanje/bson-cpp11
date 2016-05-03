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
enum Type_t {

  // End of object
  kEOO = 0,

  // double precision floating point value
  kNumberDouble = 1,

  kString = 2,

  // an embedded object
  kObject = 3,

  // an embedded array
  kArray = 4,

  // boolean type
  kBoolean = 8,

  kNull = 10,

  // 32-bit integers
  kNumberInt = 16,

  // UTC datetime
  kDatetime = 17,

  // 64-bit integers
  kNumberLong = 18,
};

extern const char *TypeToString(Type_t t);

//
// Type traits
//

class Object;
class Array;

template <class T> struct is_valid_type : public std::false_type {};
template <> struct is_valid_type<double> : public std::true_type {};
template <> struct is_valid_type<int> : public std::true_type {};
template <> struct is_valid_type<long long> : public std::true_type {};
template <> struct is_valid_type<Object> : public std::true_type {};
template <> struct is_valid_type<Array> : public std::true_type {};
template <> struct is_valid_type<bool> : public std::true_type {};
template <> struct is_valid_type<std::nullptr_t> : public std::true_type {};

inline bool IsValidType(Type_t type) {
  switch (type) {
    case kEOO:
    case kNumberInt:
    case kNumberLong:
    case kNumberDouble:
    case kNull:
    case kBoolean:
    case kString:
    case kObject:
    case kArray:
    case kDatetime:
      return true;
    default:
      return false;
  }
}

}  // namespace bson