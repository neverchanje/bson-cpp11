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

#include "DisallowCopying.h"
#include "Slice.h"

namespace bson {

// Status represents the result of an operation. It can be either a success or
// an error state.
//
class Status {
 private:
  enum ErrorCodes { kOK = 0, kFailedToParse = 1 };

 public:
  Status() = default;

  // copyable
  Status(const Status& rhs);
  Status& operator=(const Status& rhs);

  // Return an success state.
  //
  // The cost of creating an success state is much cheaper than an error state.
  // The error information is generated only when the Status object is not an
  // OK object, which contains merely a pointer.
  //
  static Status OK() { return Status(); }
  bool IsOK() const { return code() == ErrorCodes::kOK; }

  static Status FailedToParse(const Slice& msg) {
    return Status(ErrorCodes::kFailedToParse, msg);
  }
  bool IsFailedToParse() const { return code() == ErrorCodes::kFailedToParse; }

  std::string ToString() const;

 private:
  Status(ErrorCodes errorCode, const Slice& msg)
      : info_(new ErrorInfo(errorCode, msg)) {}

  struct ErrorInfo {
    unsigned char code;
    std::string msg;

    ErrorInfo(ErrorCodes c, const Slice& s)
        : code(static_cast<unsigned char>(c)), msg(s.RawData(), s.Len()) {}
  };

  ErrorCodes code() const {
    if (!info_) return ErrorCodes::kOK;
    return static_cast<ErrorCodes>(info_->code);
  }

  void copy(const Status&);

 private:
  std::unique_ptr<ErrorInfo> info_;
};

inline Status& Status::operator=(const Status& status) {
  copy(status);
  return (*this);
}

inline Status::Status(const Status& status) { copy(status); }

}  // namespace bson