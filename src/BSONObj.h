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
 *
 * A BSON object is an unordered set of name/value pairs.
 *
 * BSON object format:
 * <unsigned totalSize> {<byte BSONType><cstring FieldName><Data>}* EOO
 *
 * EOO: End Of Object
 */
class BSONObj {
 public:
  class Iterator;

  void Dump() const;

  // empty object
  inline BSONObj();

 private:
  const char* code_;
};

class BSONObj::Iterator {
  // copyable

 private:
  const char* pos_;
};

}  // namespace bson