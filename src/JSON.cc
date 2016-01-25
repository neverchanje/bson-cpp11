//
// Created by neverchanje on 1/22/16.
//

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

  explicit JSONParser(const std::string &json);

  void Parse(BSONObjBuilder &builder);

  inline Status ParseError(const Slice &msg) {
    return Status::FailedToParse(msg);
  }

 private:

  /**
   * Returns true iff the next non-whitespace sequence in the buffer matches
   * the given token, and false iff we reach the end of our buffer or the
   * token doesn't match.
   *
   * Updates the pos_ pointer when token is matched.
   */
  bool advance(const char *token);

 private:
  const char *buf_ const; // the input buffer
  const char *pos_; // current position of the buffer
};

void JSONParser::Parse(BSONObjBuilder &builder) {
  if (advance(LBRACE)) {

  }
}

JSONParser::JSONParser(const std::string &json) :
    buf_(json.c_str()),
    pos_(&buf_[0]) {
}

BSONObj FromJSON(const std::string &json) {
  JSONParser parser(json);
  BSONObjBuilder builder;

  parser.Parse(builder);

  return builder.Obj();
}

bool JSONParser::advance(const char *token) {

}

} // namespace bson
