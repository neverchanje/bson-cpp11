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

  Status parseError(const Slice &msg) { return Status::FailedToParse(msg); }

 private:
  const char *buf_ const; // the input buffer
  const char *pos_; // current position of the buffer
  const char *buf_end_ const; // the end of the input buffer
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

  Status ret = parser.Parse(builder);

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

  std::string firstField;
  if (parseField(firstField).IsOK()) {

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

} // namespace bson
