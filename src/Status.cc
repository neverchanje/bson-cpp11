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

void Status::copy(const Status& rhs) {
  if (!rhs.info_) {
    info_.release();
  } else if (!info_) {
    info_ =
        std::unique_ptr<ErrorInfo>(new ErrorInfo(rhs.code(), rhs.info_->msg));
  } else {
    info_->code = rhs.info_->code;
    info_->msg = rhs.info_->msg;
  }
}

}  // namespace bson