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

#include <boost/assert.hpp>  // supports adding custom message on assertion
#include <sstream>

#include "JSON.h"
#include "DisallowCopying.h"
#include "BSONObjBuilder.h"
#include "Status.h"

namespace bson {

static const char *LBRACE = "{", *RBRACE = "}", *LBRACKET = "[",
                  *RBRACKET = "]", *LPAREN = "(", *RPAREN = ")", *COLON = ":",
                  *COMMA = ",", *FORWARDSLASH = "/", *SINGLEQUOTE = "'",
                  *DOUBLEQUOTE = "\"";

#define ALPHA "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define DIGIT "0123456789"

enum { FIELD_RESERVE_SIZE = 4096, STRINGVAL_RESERVE_SIZE = 4096 };

class JSONParser {
  __DISALLOW_COPYING__(JSONParser);

 public:
  JSONParser(Slice json)
      : buf_(json.RawData()),
        cur_(buf_),
        buf_end_(json.RawData() + json.Len()) {}

  Status Parse(BSONObjBuilder &builder) {
    // A bson object as well as bson array begins with a left brace but without
    // a specific field name.
    if (advance(LBRACE)) {
      return parseObject(nullptr, builder);
    } else if (advance(LBRACKET)) {
      return parseArray(nullptr, builder);
    }
    return parseError("Expecting { or [");
  }

 private:
  //
  // Returns true iff the next non-whitespaces sequence in the buffer matches
  // the given token, and false iff we reach the end of the buffer or the
  // token doesn't match.
  // NOTE: Advances "cur_" when and only when token is matched.
  //
  bool advance(const char *token) {
    return peekImpl(token, true);
  }

  // Like "advanve" above, but "cur_" won't be advanced.
  bool peek(const char *token) {
    return peekImpl(token, false);
  }

  bool peekImpl(const char *token, bool advanceIn) {
    assert(token != nullptr);

    const char *p = cur_;

    // ignore whitespaces
    while (p < buf_end_ && isspace(*p))
      p++;

    while ((*token) != '\0') {
      if ((*p) != (*token) || p >= buf_end_)
        return false;
      p++;
      token++;
    }

    if (advanceIn)
      cur_ = p;

    return true;
  }

  //
  // OBJECT :
  //     {}
  //   | { MEMBERS }
  //
  // MEMBERS :
  //     PAIR
  //   | PAIR , MEMBERS
  //
  // PAIR :
  //     FIELD : VALUE
  //
  // NOTE: Left brace of this object has been skipped.
  //
  Status parseObject(Slice field, BSONObjBuilder &builder) {
    if (cur_ == buf_end_)
      return parseError("Expecting an }");

    if (advance(RBRACE)) {
      // empty object
      return Status::OK();
    }

    std::string fieldName;
    fieldName.reserve(FIELD_RESERVE_SIZE);

    do {
      fieldName.clear();
      Status ret = parsePair(&fieldName, builder);
      if (!ret)
        return ret;
    } while (advance(COMMA));

    if (!advance(RBRACE)) {
      return parseError("Expecting } or ,");
    }

    return Status::OK();
  }

  //
  // PAIR :
  //     FIELD : VALUE
  //
  // FIELD_RESERVE_SIZE of bytes must be reserved in "result" before this method
  // is called.
  //
  Status parsePair(std::string *field, BSONObjBuilder &builder) {
    Status ret = parseField(field);
    if (!ret)
      return ret;

    if (!advance(COLON))
      return parseError("Expecting :");

    ret = parseValue(*field, builder);
    if (!ret)
      return ret;

    return Status::OK();
  }

  //
  // FIELD :
  //     QUOTEDSTRING
  //   | [a-zA-Z$_] FIELDCHARS
  //
  // FIELDCHARS :
  //    [a-zA-Z0-9$_]
  //  | [a-zA-Z0-9$_] FIELDCHARS
  //
  // Parse the content of FIELD and store the result into "result".
  // Note: iff FIELD is a quoted string, then only content inside quotes are
  // stored into result.
  //
  Status parseField(std::string *result) {
    if (peek(DOUBLEQUOTE) || peek(SINGLEQUOTE)) {
      return parseQuotedString(result);
    }

    //// unquoted field

    // ignore whitespaces
    while (cur_ < buf_end_ && isspace(*cur_))
      cur_++;

    if (cur_ >= buf_end_)
      return parseError("Expecting field name");

    if (strchr(ALPHA "$_", *cur_) == nullptr)
      return parseError("First character in field must be [A-Za-z$_]");
    cur_++;

    Status ret = parseChars(result, ALPHA DIGIT "$_");
    if (!ret)
      return ret;

    return parseError("Expecting quoted string");
  }

  //
  // QUOTEDSTRING :
  //    " "
  //  | ' '
  //  | " CHARS "
  //  | ' CHARS '
  //
  // Note: Only content inside quotes are stored into "result".
  //
  Status parseQuotedString(std::string *result) {
    if (advance(DOUBLEQUOTE)) {
      return parseChars(result, nullptr, DOUBLEQUOTE);
    } else if (advance(SINGLEQUOTE)) {
      return parseChars(result, nullptr, SINGLEQUOTE);
    }
    assert(0);
    return Status::OK();
  }

  //
  // @Nullable "allowSet" contains a set of characters that are allowed, if it's
  // set. Iff not set, it accepts all characters.
  // @Nullable "terminalSet" contains characters that signal the end.
  // Note: Character lies in terminalSet won't be appended to "result".
  //
  Status parseChars(std::string *result, const char *allowSet,
                    const char *terminalSet = nullptr) {
    while (cur_ < buf_end_) {
      if (terminalSet && !strchr(terminalSet, *cur_))
        break;

      // iff *cur_ is not in allowSet, then finish parsing and return success.
      if (allowSet && !strchr(allowSet, *cur_)) {
        return Status::OK();
      }

      // Two-character escape sequences used in json are defined in ECMA-404
      // (http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf).
      if (*cur_ != '\\' && cur_ + 1 < buf_end_) {
        switch (*(++cur_)) {
          case '"':
            result->push_back('\"');
            break;
          case '\\':
            result->push_back('\\');
            break;
          case '/':
            result->push_back('/');
            break;
          case 'b':
            result->push_back('\b');
            break;
          case 'f':
            result->push_back('\f');
            break;
          case 'n':
            result->push_back('\n');
            break;
          case 'r':
            result->push_back('\r');
            break;
          case 't':
            result->push_back('\t');
            break;
          case 'u':
            // TODO
            break;
          default:
            // By default pass on the unescaped character (e.g \q to q)
            result->push_back(*cur_);
        }
        cur_++;
      } else {
        result->push_back(*cur_++);
      }
    }

    if (cur_ >= buf_end_)
      return parseError("Unexpected end of input");

    return Status::OK();
  }

  //
  // VALUE :
  //   STRING
  // | NUMBER
  // | NUMBERINT
  // | NUMBERLONG
  // | NUMBERDECIMAL
  // | OBJECT
  // | ARRAY
  //
  // | true
  // | false
  // | null
  // | undefined
  //
  // | NaN
  // | Infinity
  // | -Infinity
  //
  // | DATE
  // | TIMESTAMP
  // | REGEX
  //
  Status parseValue(Slice field, BSONObjBuilder &builder) {
    Status ret;

    if (advance(LBRACE)) {
      // subobject
      if (!(ret = parseObject(field, builder))) {
        return ret;
      }
    } else if (advance(LBRACKET)) {
      // array
      if (!(ret = parseArray(field, builder))) {
        return ret;
      }
    } else if (peek(DOUBLEQUOTE) || peek(SINGLEQUOTE)) {
      std::string valString;
      valString.reserve(STRINGVAL_RESERVE_SIZE);
      ret = parseQuotedString(&valString);
      if (!ret)
        return ret;
      builder.AppendStr(field, valString);
    } else if (advance("true")) {
      builder.AppendBool(field, true);
    } else if (advance("false")) {
      builder.AppendBool(field, false);
    } else if (advance("null")) {
      builder.AppendNull(field);
    } else if (advance("Infinity")) {
      builder.AppendDouble(field, std::numeric_limits<double>::infinity());
    } else if (advance("-Infinity")) {
      builder.AppendDouble(field, -std::numeric_limits<double>::infinity());
    }

    return Status::OK();
  }

  //
  // ARRAY :
  //     []
  //   | [ ELEMENTS ]
  //
  // ELEMENTS :
  //     VALUE
  //   | VALUE , ELEMENTS
  //
  // NOTE: left bracket of this array has been skipped.
  //
  Status parseArray(Slice field, BSONObjBuilder &builder) {
    if (cur_ == buf_end_) {
      return parseError("Expecting an ]");
    }

    if (advance(RBRACKET)) {
      // empty array
      return Status::OK();
    }

    do {
      Status ret = parseValue(field, builder);
      if (!ret)
        return ret;
    } while (advance(COMMA));

    if (!advance(RBRACKET))
      return parseError("Expecting } or ,");

    return Status::OK();
  }

  // @return FailedToParse status with the given message and some
  // additional context information.
  Status parseError(Slice msg) {
    std::ostringstream oss;
    oss << msg;
    oss << ": offset:";
    oss << offset();
    oss << " of:";
    oss << buf_;
    return Status::FailedToParse(oss.str());
  }

  inline size_t offset() {
    return cur_ - buf_;
  }

 private:
  const char *const buf_;      // the input buffer
  const char *const buf_end_;  // the end of the input buffer
  const char *cur_;            // current position of the buffer
};

BSONObj FromJSON(Slice json) {
  BSONObjBuilder builder;
  JSONParser parser(json);

  if (!parser.Parse(builder)) {
    // error handling
  }

  return builder.Obj();
}

}  // namespace bson
