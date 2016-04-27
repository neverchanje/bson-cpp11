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

#include "Element.h"
#include "BSONObj.h"

namespace bson {

namespace internal {

using silly::IteratorFacade;
using silly::ForwardIteratorTag;

template <bool IsConst = true>
class BSONObjIterator : public IteratorFacade<BSONObjIterator<IsConst>, Element,
                                              ForwardIteratorTag, IsConst> {
  typedef IteratorFacade<BSONObjIterator<IsConst>, Element, ForwardIteratorTag,
                         IsConst> Facade;
  typedef typename Facade::Reference Reference;

 public:
  BSONObjIterator(const char *data, const BSONObj &obj)
      : pos_(data), obj_(&obj) {}

  BSONObjIterator(const BSONObjIterator &other)
      : pos_(other.pos_), obj_(other.obj_) {}

  const BSONObjIterator &operator=(const BSONObjIterator &other) {
    pos_ = other.pos_;
    obj_ = other.obj_;
    return *this;
  }

 private:
  //
  // The following functions are required for using silly::IteratorFacade.
  //
  friend class silly::IteratorCoreAccess;

  // Lazy dereference.
  Reference dereference() const {
    if (!e_ || e_->RawData() != pos_)
      e_.reset(new Element(pos_));
    return *e_;
  }

  void increment() {
    //    assert(pos_ < obj_->end_);
    pos_ += Element(pos_).Size();
  }

  bool equal(const BSONObjIterator &other) const {
    return pos_ == other.pos_;
  }

 private:
  const char *pos_;
  const BSONObj *obj_;
  mutable std::unique_ptr<Element> e_;
};

}  // namespace internal

}  // namespace bson