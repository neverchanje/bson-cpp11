//
// Created by neverchanje on 1/23/16.
//

#pragma once

#include "Slice.h"

namespace bson {

// Status represents the result of an operation. It can be either a success or
// an error state.
//
class Status {

 public:

  // Return an success state.
  // The cost of creating an success state is much cheaper than an error state.
  // The internal error infomation is generated only when the Status object is
  // not an OK object.
  static Status OK() { return Status(); }
  bool IsOK() const { return Code() == ErrorCodes::OK; }

  static Status FailedToParse(const Slice &msg) { return Status(ErrorCodes::FailedToParse, msg); }
  bool IsFailedToParse() const { return Code() == ErrorCodes::FailedToParse; }

  ErrorCodes Code() const {
    if (!info_)
      return ErrorCodes::OK;
    return static_cast<ErrorCodes>(info_->code);
  }

  std::string ToString() const;

 private:

  Status() = default;

  Status(ErrorCodes errorCode, const Slice &msg) :
      info_(new ErrorInfo(errorCode, msg)) {
  }

  enum ErrorCodes {
    OK = 0,
    FailedToParse = 1
  };

  struct ErrorInfo {
    unsigned char code;
    std::string msg;

    ErrorInfo(ErrorCodes c, const Slice &s) :
        code(static_cast<unsigned char>(c)),
        msg(s.RawData(), s.Len()) {
    }
  };

  const std::unique_ptr<ErrorInfo> info_;
};

} // namespace bson