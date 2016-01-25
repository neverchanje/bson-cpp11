//
// Created by neverchanje on 1/22/16.
//


#pragma once

#include <cstddef>

namespace bson {

class BSONElement {

 public:

  // Size of element.
  size_t Size() const;

 private:

  const char *elem_;
};

} // namespace bson
