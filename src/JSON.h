//
// Created by neverchanje on 1/22/16.
//


#pragma once

#include <string>

namespace bson {

class BSONObj;

BSONObj FromJSON(const std::string &json);

std::string ToJSON(const BSONObj &bson);

} // namespace bson
