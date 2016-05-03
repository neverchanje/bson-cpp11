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

#include "Element.h"
#include "Slice.h"
#include "internal/ObjectIterator.h"

namespace bson {

// A BSON object is an unordered set of name/value pairs. @see ObjectBuilder.h
// for the format of BSON object.
// Object represents a block of binary data constructed from ObjectBuilder.
// The lifetime of the internal binary data is managed by reference counting, if
// no instance of Object or BSONBuilder holds the data, it will be freed.
//

typedef std::shared_ptr<const char> SharedBuffer;

class Object {
  static const size_t SZ_TotalSize = 4;
  static const size_t SZ_EOO = 1;

  template <bool IsConst> friend class internal::ObjectIterator;

 public:
  Object(const SharedBuffer &sharedBuf)
      : sbuf_(sharedBuf), data_(sharedBuf.get()), end_(data_) {
    end_ = data_ + ConstDataView(data_).ReadNum<int>() - SZ_EOO;
  }

  // intentionally copyable

  // (DEBUG)
  std::string Dump() const;

 public:
  //
  // "begin" and "end" function in stdlib-style, so that we're able to use the
  // syntactic sugar of range-based loop.
  //

  typedef internal::ObjectIterator<true> ConstIterator;
  typedef internal::ObjectIterator<false> Iterator;

  Iterator begin() {
    return Iterator(data_ + SZ_TotalSize, *this);
  }

  ConstIterator begin() const {
    return ConstIterator(data_ + SZ_TotalSize, *this);
  }

  Iterator end() {
    return Iterator(end_, *this);
  }

  ConstIterator end() const {
    return ConstIterator(end_, *this);
  }

  // Search the bson object for an element of the specified field name, if found
  // it returns an iterator, otherwise it returns an iterator to Object::end().
  Iterator find(Slice field) {
    for (auto it = begin(); it != end(); it++) {
      if (strcmp(field.RawData(), it->RawFieldName()) == 0) {
        return it;
      }
    }
    return end();
  }

  ConstIterator find(Slice field) const {
    for (auto it = begin(); it != end(); it++) {
      if (strcmp(field.RawData(), it->RawFieldName()) == 0) {
        return it;
      }
    }
    return end();
  }

 public:
  //
  // Observers.
  //

  // @return the raw data of the object
  const char *RawData() const {
    return data_;
  }

  // @return the number of top level fields in the object
  // NOTE: takes O(n) time iteration to count the fields
  int NumFields() const {
    int ret = 0;
    for (auto it = begin(); it != end(); it++)
      ret++;
    return ret;
  }

  bool HasMember(Slice field) const {
    return find(field) != end();
  }

  Element &operator[](Slice field) {
    return *find(field);
  }

  // @return total size of the BSON object in bytes.
  size_t TotalSize() const {
    return static_cast<size_t>(ConstDataView(data_).ReadNum<int>());
  }

  const SharedBuffer &ShareFromThis() const {
    return sbuf_;
  }

 private:
  const char *data_;
  const char *end_;
  SharedBuffer sbuf_;
};

struct Array : public Object {
  Array(const SharedBuffer &sharedBuf) : Object(sharedBuf) {}
  explicit Array(const Object &obj) : Array(obj.ShareFromThis()) {}
};

}  // namespace bson