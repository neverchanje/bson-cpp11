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

#include <silly/IteratorFacade.h>

#include "BSONElement.h"
#include "Slice.h"

namespace bson {

// A BSON object is an unordered set of name/value pairs.
//
// BSON object format: @see BSONObjBuilder.h
//

class BSONObj {
  static const size_t SZ_TotalSize = 4;
  static const size_t SZ_EOO = 1;

 public:
  BSONObj(const char *bson_data)
      : data_(bson_data),
        end_(data_ + ConstDataView(data_).ReadNum<int>() - SZ_EOO) {}

  // intentionally copyable

  // (DEBUG)
  std::string Dump() const;

 public:
  class Iterator : public silly::IteratorFacade<Iterator, BSONElement const,
                                                silly::ForwardIteratorTag> {
    friend class BSONObj;

   public:
    Iterator(const Iterator &other) : pos_(other.pos_), obj_(other.obj_) {}

    const Iterator &operator=(const Iterator &other) {
      pos_ = other.pos_;
      obj_ = other.obj_;
      return *this;
    }

   private:
    // BSONObj::Iterator can only be constructed inside BSONObj.
    Iterator(const char *data, const BSONObj &obj) : pos_(data), obj_(&obj) {}

    //
    // The following functions are required for using silly::IteratorFacade.
    //
    friend class silly::IteratorCoreAccess;

    // Lazy dereference.
    const BSONElement &dereference() const {
      if (!e_ || e_->RawData() != pos_)
        e_.reset(new BSONElement(pos_));
      return *e_;
    }

    void increment() {
      assert(pos_ < obj_->end_);
      pos_ += BSONElement(pos_).Size();
    }

    bool equal(const Iterator &other) const {
      return pos_ == other.pos_;
    }

   private:
    const char *pos_;
    const BSONObj *obj_;
    mutable std::unique_ptr<BSONElement> e_;
  };

  //
  // "begin" and "end" function in stdlib-style, so that we're able to use the
  // syntactic sugar of range-based loop.
  //

  Iterator begin() const {
    return Iterator(data_ + SZ_TotalSize, *this);
  }

  Iterator end() const {
    return Iterator(end_, *this);
  }

  // Search the bson object for an element of the specified field name, if found
  // it returns an iterator, otherwise it returns an iterator to BSONObj::End().
  Iterator find(Slice field) const {
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
  // NOTE: takes O(n) time to iterate to count the fields
  int NumFields() const {
    int ret = 0;
    for (auto it = begin(); it != end(); it++)
      ret++;
    return ret;
  }

  // @return total size of the BSON object in bytes.
  size_t TotalSize() const {
    return static_cast<size_t>(ConstDataView(data_).ReadNum<int>());
  }

 private:
  const char *data_;
  const char *end_;
};

struct BSONArray : public BSONObj {
  BSONArray(const char *data) : BSONObj(data) {}
  explicit BSONArray(const BSONObj obj) : BSONArray(obj.RawData()) {}
};

}  // namespace bson