//
// Created by neverchanje on 1/23/16.
//

#include "Status.h"

namespace bson {

std::string Status::ToString() const {
  if (!info_) return "OK";

  std::string ret;

  switch (code()) {
    case ErrorCodes::kFailedToParse:
      ret = "FailedToParse";
      break;
    default:
      ret = "Unknown ErrorCode";
      break;
  }

  return ret + ": " + info_->msg;
}

void Status::copy(const Status &rhs) {
  if (!rhs.info_) {
    info_.release();
  } else if (!info_) {
    info_ = std::unique_ptr<ErrorInfo>(new ErrorInfo(rhs.code(), rhs.info_->msg));
  } else {
    info_->code = rhs.info_->code;
    info_->msg = rhs.info_->msg;
  }
}

} // namespace bson