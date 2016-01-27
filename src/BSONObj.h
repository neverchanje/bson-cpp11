//
// Created by neverchanje on 1/22/16.
//


#pragma once

namespace bson {

/**
 *
 * A BSON object is an unordered set of name/value pairs.
 *
 * BSON object format:
 * <unsigned totalSize> {<byte BSONType><cstring FieldName><Data>}* EOO
 *
 * EOO: End Of Object
 */
class BSONObj {

 public:

  class Iterator;

  void Dump() const;

  // empty object
  inline BSONObj();

 private:
  const char *code_;
};

class BSONObj::Iterator {
// copyable


 private:
  const char *pos_;
};

} // namespace bson