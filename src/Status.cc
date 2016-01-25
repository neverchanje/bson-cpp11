//
// Created by neverchanje on 1/23/16.
//

#include "Status.h"

namespace bson {

std::string Status::ToString() const {
  std::string ret;

  switch (Code()) {
    case Status::ErrorCodes::OK:
      ret += "OK";
      break;
    case Status::ErrorCodes::FailedToParse:
      ret += "FailedToParse";
      break;
    default:
      ret += "Unknown Code";
      break;
  }

  return ret + info_.msg;
}

} // namespace bson