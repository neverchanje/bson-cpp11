//
// Created by neverchanje on 2/3/16.
//


#pragma once

namespace bson {

/**
 * The complete list of BSON types can be found on http://bsonspec.org/spec.html.
 * By now only a subset of them are implemented.
 */
enum BSONTypes {
  NumberDouble = 1, // double precision floating point value
  String = 2,
  Object = 3,
  Array = 4,
  Boolean = 8,
  Null = 10,
  NumberInt = 16, // 32-bit integers
  NumberLong = 18, // 64-bit integers
};

} // namespace bson