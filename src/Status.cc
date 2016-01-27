//
// Created by neverchanje on 1/23/16.
//

#include "Status.h"

namespace bson {

std::string Status::ToString() const {
  std::string ret;

  switch (Code()) {
    case ErrorCodes::kOK:
      ret = "OK";
      break;
    case ErrorCodes::kFailedToParse:
      ret = "FailedToParse";
      break;
    default:
      ret = "Unknown ErrorCode";
      break;
  }

  return ret + info_->msg;
}

} // namespace bson