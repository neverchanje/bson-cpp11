//
// Created by neverchanje on 1/24/16.
//

#pragma once

#include <string>

namespace bson {

// A Slice object wraps a "const char *" or a "const std::string&" but
// without copying their contents. It's used when passed as a function
// argument that takes any of the two forms above.
//
// A Slice doesn't hold the ownership of the string.
//
class Slice {

 public:

  Slice(const std::string &s) :
      Slice(s.data(), s.size()) {
  }

  Slice(const char *s) :
      Slice(s, strlen(s)) {
  }

  constexpr Slice(std::nullptr_t p) :
      str_(nullptr),
      len_(0) {
  }

  char operator[](size_t n) const { return str_[n]; }

  size_t Len() const { return len_; }

  const char *RawData() const { return str_; }

  bool Empty() const { return len_ == 0; }

  int Compare(const Slice &rhs) const {
    return memcmp(str_, rhs.RawData(), len_);
  }

 private:

  Slice(const char *s, size_t n) :
      str_(s),
      len_(n) {
  }

 private:
  const char *str_;
  const size_t len_;
};

inline bool operator==(const Slice &lhs, const Slice &rhs) {
  return lhs.Compare(rhs) == 0;
}

inline std::ostream &operator<<(std::ostream &stream, const Slice &s);

} // namespace bson