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

#include <boost/assert.hpp>

#include "Type.h"

namespace bson {

const char *TypeToString(Type_t t) {
  switch (t) {
    case kEOO:
      return "EOO";
    case kNumberDouble:
      return "NumberDouble";
    case kString:
      return "kString";
    case kObject:
      return "Object";
    case kArray:
      return "Array";
    case kBoolean:
      return "Boolean";
    case kNull:
      return "Null";
    case kNumberInt:
      return "NumberInt";
    case kNumberLong:
      return "NumberLong";
    case kDatetime:
      return "Datetime";
    default:
      BOOST_ASSERT_MSG(0, "Unknown type");
      return nullptr;
  }
}

}  // namespace bson