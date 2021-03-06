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

#include <sstream>

#include "Object.h"

namespace bson {

std::string Object::Dump() const {
  std::ostringstream oss;
  for (ConstIterator i = begin(); i != end(); i++) {
    const Element &e = *i;
    oss << "Field: " << e.RawFieldName() << ", Type: " << TypeToString(e.Type())
        << "\n";
  }
  return oss.str();
}

}  // namespace bson
