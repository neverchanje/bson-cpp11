//
// Created by neverchanje on 1/23/16.
//

#pragma once

#include "Slice.h"
#include "DisallowCopying.h"

namespace bson {

// Status represents the result of an operation. It can be either a success or
// an error state.
//
class Status {

 private:

  enum ErrorCodes {
    kOK = 0,
    kFailedToParse = 1
  };

 public:

  Status() = default;

  // copyable
  Status(const Status &rhs);
  Status &operator=(const Status &rhs);

  // Return an success state.
  //
  // The cost of creating an success state is much cheaper than an error state.
  // The error information is generated only when the Status object is not an
  // OK object, which contains merely a pointer.
  //
  static Status OK() { return Status(); }
  bool IsOK() const { return code() == ErrorCodes::kOK; }

  static Status FailedToParse(const Slice &msg) { return Status(ErrorCodes::kFailedToParse, msg); }
  bool IsFailedToParse() const { return code() == ErrorCodes::kFailedToParse; }

  std::string ToString() const;

 private:

  Status(ErrorCodes errorCode, const Slice &msg) :
      info_(new ErrorInfo(errorCode, msg)) {
  }

  struct ErrorInfo {
    unsigned char code;
    std::string msg;

    ErrorInfo(ErrorCodes c, const Slice &s) :
        code(static_cast<unsigned char>(c)),
        msg(s.RawData(), s.Len()) {
    }
  };

  ErrorCodes code() const {
    if (!info_)
      return ErrorCodes::kOK;
    return static_cast<ErrorCodes>(info_->code);
  }

  void copy(const Status &);

 private:
  std::unique_ptr<ErrorInfo> info_;
};

inline Status &Status::operator=(const Status &status) {
  copy(status);
  return (*this);
}

inline Status::Status(const Status &status) {
  copy(status);
}

} // namespace bson