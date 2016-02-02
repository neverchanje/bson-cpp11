//
// Created by neverchanje on 1/22/16.
//

#include <cassert>

#include "JSON.h"
#include "DisallowCopying.h"
#include "BSONObjBuilder.h"
#include "Status.h"

namespace bson {

static const char
    *LBRACE = "{", *RBRACE = "}",
    *LBRACKET = "[", *RBRACKET = "]",
    *LPAREN = "(", *RPAREN = ")",
    *COLON = ":", *COMMA = ",", *FORWARDSLASH = "/",
    *SINGLEQUOTE = "'", *DOUBLEQUOTE = "\"";

enum {
  FIELD_RESERVE_SIZE = 4096
};

class JSONParser {
  __DISALLOW_COPYING__(JSONParser);

 public:

  explicit JSONParser(const Slice &json);

  Status Parse(BSONObjBuilder &builder);

 private:

  /**
   * Returns true iff the next non-whitespace sequence in the buffer matches
   * the given token, and false iff we reach the end of our buffer or the
   * token doesn't match.
   *
   * Updates the pos_ pointer when token is matched.
   */
  bool advance(const char *token);

  /*
   * OBJECT :
   *     {}
   *   | { MEMBERS }
   *
   * MEMBERS :
   *     PAIR
   *   | PAIR , MEMBERS
   *
   * PAIR :
   *     FIELD : VALUE
   *
   * FIELD:
   *     DOUBLEQUOTE CHARS DOUBLEQUOTE
   *   | SINGLEQUOTE CHARS SINGLEQUOTE
   */
  Status parseObject(const Slice &fieldName, BSONObjBuilder &builder);
  Status parseField(std::string &field);
  Status parseChars(std::string &result, const char *terminalSet);
  Status parseValue(const std::string &field, BSONObjBuilder &builder);

  // @param field must be reserved FIELD_RESERVE_SIZE bytes before this method
  // is called.
  Status parsePair(std::string &field, BSONObjBuilder &builder);

  Status parseError(const Slice &msg) { return Status::FailedToParse(msg); }

 private:
  const char *buf_; // the input buffer
  const char *pos_; // current position of the buffer
  const char *buf_end_; // the end of the input buffer
};

Status JSONParser::Parse(BSONObjBuilder &builder) {
  if (advance(LBRACE)) {
    // A bson object begins with a left brace but without a specific field name.
    return parseObject(nullptr, builder);
  }
  return parseError("Expecting {");
}

JSONParser::JSONParser(const Slice &json) :
    buf_(json.RawData()),
    pos_(buf_),
    buf_end_(json.RawData() + json.Len()) {
}

BSONObj FromJSON(const Slice &json) {
  JSONParser parser(json);
  BSONObjBuilder builder;

  if (!parser.Parse(builder).IsOK()) {
    // error handling
  }

  return builder.Obj();
}

bool JSONParser::advance(const char *token) {
  assert(token != nullptr);

  // ignore whitespaces
  while (pos_ != buf_end_ && isspace(*pos_)) pos_++;

  while (pos_ != buf_end_ && (*token) != '\0' && (*pos_) == (*token)) {
    pos_++;
    token++;
  }

  return (*token) == '\0';
}

Status JSONParser::parseObject(const Slice &fieldName, BSONObjBuilder &builder) {
  if (pos_ == buf_end_)
    return Status::FailedToParse("Expecting an }");

  std::string field;
  field.reserve(FIELD_RESERVE_SIZE);

  // parse the first pair
  Status firstRet = parseField(field);
  if (!firstRet.IsOK()) return firstRet;

  while (advance(COMMA)) {
    field.clear();
    Status ret = parseField(field);
    if (!ret.IsOK())
      return ret;
  }

  if (advance(RBRACE)) {
    return parseError("Expecting } or ,");
  }


  return Status::OK();
}

Status JSONParser::parseField(std::string &field) {
  if (advance(DOUBLEQUOTE)) {
    parseChars(field, "\"");
  } else if (advance(SINGLEQUOTE)) {
    parseChars(field, "'");
  }
  return Status::FailedToParse("Expecting quoted string");
}

Status JSONParser::parseChars(std::string &result, const char *terminalSet) {
  for (; pos_ != buf_end_; pos_++) {
    strchr(terminalSet, (*pos_));
  }
}

Status JSONParser::parseValue(const std::string &field,
                              BSONObjBuilder &builder) {
  if (advance(LBRACE)) {
    // subobject
    Status ret = parseObject(field, builder);
  } else if (advance(LBRACKET)) {
    // array
  } else if (advance(DOUBLEQUOTE)) {
    // double-quoted string
  } else if (advance(SINGLEQUOTE)) {
    // single-quoted string
  } else if (advance("true")) {

  } else if (advance("false")) {

  } else if (advance("null")) {

  } else {
    // number
  }
}

Status JSONParser::parsePair(std::string &field, BSONObjBuilder &builder) {
  Status ret = parseField(field);
  if (!ret.IsOK()) return ret;

  if (advance(COLON))
    return parseError("Expecting :");

  Status valRet = parseValue(field, builder);
  if (!valRet.IsOK()) return valRet;

  return Status::OK();
}

} // namespace bson
