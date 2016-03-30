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

namespace bson {

/**
 * The complete list of BSON types can be found on
 * http://bsonspec.org/spec.html.
 * By now only a subset of them are implemented.
 */
enum BSONTypes {
  NumberDouble = 1,  // double precision floating point value
  String = 2,
  Object = 3,
  Array = 4,
  Boolean = 8,
  Null = 10,
  NumberInt = 16,   // 32-bit integers
  NumberLong = 18,  // 64-bit integers
};

}  // namespace bson