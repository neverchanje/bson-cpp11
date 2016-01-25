//
// Created by neverchanje on 1/24/16.
//

#include "Slice.h"

#include <ostream>

namespace bson {

inline std::ostream &operator<<(std::ostream &stream, const Slice &s) {
  return stream << s.RawData();
}

} // namespace bson