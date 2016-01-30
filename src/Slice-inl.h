//
// Created by neverchanje on 1/29/16.
//

#include "Slice.h"

#pragma once

namespace bson {

inline bool operator==(const Slice &lhs, const Slice &rhs) {
  return lhs.Compare(rhs) == 0;
}

inline std::ostream &operator<<(std::ostream &stream, const Slice &s) {
  return stream << s.RawData();
}

} // namespace bson