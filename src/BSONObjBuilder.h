//
// Created by neverchanje on 1/22/16.
//


#pragma once

#include "DisallowCopying.h"
#include "BSONObj.h"
#include "Slice.h"

namespace bson {

class BSONObjBuilder {
  __DISALLOW_COPYING__(BSONObjBuilder);

 public:

  BSONObj Obj() const { return obj_; };

  BSONObjBuilder() = default;

  void BuildObject();

  void BuildValue();

  void BuildField();

 private:
  BSONObj obj_;
};

} // namespace bson
