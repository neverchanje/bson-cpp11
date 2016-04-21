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

#pragma once

#include <boost/assert.hpp>
#include <sstream>

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

class BSONParser {
  __DISALLOW_COPYING__(BSONParser);

 public:
  BSONParser(Slice json)
      : buf_(json.RawData()),
        cur_(buf_),
        buf_end_(json.RawData() + json.Len()) {}

  Status Parse(BSONObjBuilder &builder) {
    // A bson object as well as bson array begins with a left brace but without
    // a specific field name.
    if (advance(LBRACE)) {
      return parseObject(nullptr, builder, false);
    } else if (advance(LBRACKET)) {
      return parseArray(nullptr, builder, false);
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
  Status parseObject(Slice field, BSONObjBuilder &builder, bool subObj = true) {
    if (cur_ == buf_end_)
      return parseError("Expecting an }");

    std::unique_ptr<BSONObjBuilder> subBuilder;
    BSONObjBuilder *objBuilder = &builder;
    if (subObj) {
      subBuilder.reset(new BSONObjBuilder());
      objBuilder = subBuilder.get();
    }

    if (advance(RBRACE)) {
      // empty object
      // TODO: Optimization on building empty objects.
      objBuilder->DoneFast();
      if (subObj) {
        builder.Append(field, subBuilder->Obj());
      }
      return Status::OK();
    }

    std::string fieldName;
    fieldName.reserve(FIELD_RESERVE_SIZE);

    do {
      fieldName.clear();
      Status ret = parsePair(&fieldName, *objBuilder);
      if (!ret)
        return ret;
    } while (advance(COMMA));

    if (!advance(RBRACE)) {
      return parseError("Expecting } or ,");
    }

    objBuilder->DoneFast();
    if (subObj) {
      builder.Append(field, subBuilder->Obj());
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
  // stored into "result".
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
  Status parseQuotedString(std::string *result) {
    Status r;
    if (advance(DOUBLEQUOTE)) {
      result->push_back('"');
      r = parseChars(result, nullptr, DOUBLEQUOTE);
      if (!r)
        result->push_back('"');
    } else if (advance(SINGLEQUOTE)) {
      result->push_back('\'');
      r = parseChars(result, nullptr, SINGLEQUOTE);
      if (!r)
        result->push_back('\'');
    }
    return r;
  }

  //
  // @Nullable "allowSet" contains a set of characters that are allowed, if it's
  // set. Iff not set, it accepts all characters.
  // @Nullable "terminalSet" contains characters that signal the end.
  // Note: Character lies in "terminalSet" won't be appended to "result".
  //
  Status parseChars(std::string *result, const char *allowSet,
                    const char *terminalSet = nullptr) {
    while (cur_ < buf_end_) {
      if (terminalSet && strchr(terminalSet, *cur_)) {
        cur_++;  // Skip this terminated character.
        break;
      }

      // iff *cur_ is not in allowSet, then finish parsing and return success.
      if (allowSet && !strchr(allowSet, *cur_)) {
        return Status::OK();
      }

      // Two-character escape sequences used in json are defined in ECMA-404
      // (http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf).
      if (*cur_ == '\\' && cur_ + 1 < buf_end_) {
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
  //
  // | Infinity
  // | -Infinity
  //
  // | DATE
  // | REGEX
  //
  Status parseValue(Slice field, BSONObjBuilder &builder) {
    Status ret;

    if (advance("Datetime")) {
      // Datetime
      if (!(ret = parseDatetime(field, builder))) {
        return ret;
      }
    } else if(advance("NumberInt")){
      // NumberInt
      if (!(ret = parseNumberInt(field, builder))) {
        return ret;
      }
    } else if(advance("NumberLong")){
      // NumberLong
      if (!(ret = parseNumberLong(field, builder))) {
        return ret;
      }
    } else if (advance(LBRACE)) {
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
    } else {
      Status ret;
      if (!(ret = parseNumber(field, builder)))
        return ret;
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
  // @param subObj indicates whether a new BSONObjBuilder is required.
  //
  Status parseArray(Slice field, BSONObjBuilder &builder, bool subObj = true) {
    if (cur_ == buf_end_) {
      return parseError("Expecting an ]");
    }

    std::unique_ptr<BSONObjBuilder> subBuilder;
    BSONObjBuilder *objBuilder = &builder;
    if (subObj) {
      subBuilder.reset(new BSONObjBuilder());
      objBuilder = subBuilder.get();
    }

    if (advance(RBRACKET)) {
      // empty array
      objBuilder->DoneFast();
      if (subObj) {
        builder.Append(field, BSONArray(subBuilder->Obj()));
      }
      return Status::OK();
    }

    do {
      Status ret = parseValue(nullptr, *objBuilder);
      if (!ret)
        return ret;
    } while (advance(COMMA));

    if (!advance(RBRACKET))
      return parseError("Expecting } or ,");

    objBuilder->DoneFast();
    if (subObj) {
      builder.Append(field, BSONArray(subBuilder->Obj()));
    }
    return Status::OK();
  }

  // Number:
  // Number parsing is based on standard library functions, not
  // necessarily on the JSON numeric grammar.
  //
  Status parseNumber(Slice field, BSONObjBuilder &builder) {
    Status ret;
    char *pendll, *pendd;
    int err_num;

    // ignore whitespaces
    while (cur_ < buf_end_ && isspace(*cur_))
      cur_++;

    // Try parsing double.
    // Values that can be parsed into double can be parsed into long long as
    // well.
    errno = 0;
    double dr = strtod(cur_, &pendd);
    err_num = errno;

    if (pendd == nullptr) {
      return parseError("Invalid conversion from string to number");
    }

    if (err_num == ERANGE) {
      return parseError("Value cannot fit in double");
    }

    // Try parsing long long
    errno = 0;
    long long llr = strtoll(cur_, &pendll, 10);
    err_num = errno;

    if (pendll < pendd || err_num == ERANGE) {
      builder.Append(field, dr);
      cur_ = pendd;
    } else if (llr <= std::numeric_limits<int>::max() ||
               llr >= std::numeric_limits<int>::min()) {
      builder.Append(field, static_cast<int>(llr));
      cur_ = pendll;
    } else {
      builder.Append(field, llr);
      cur_ = pendll;
    }
    return Status::OK();
  }

  // NUMBERINT :
  //   NumberInt( <number> )
  Status parseNumberInt(Slice field, BSONObjBuilder& builder) {
    if (!advance(LPAREN))
      return parseError("Expecting (");

    char *pEnd;
    int64_t val;
    int err_num;

    errno = 0;
    val = strtoll(cur_, &pEnd, 10);
    err_num = errno;

    if (pEnd == nullptr)
      return parseError("NumberInt: Invalid conversion from string to integer");

    if (err_num == ERANGE || val > std::numeric_limits<int>::max()
        || val < std::numeric_limits<int>::min())
      return parseError("NumberInt: Value cannot fit in int32");

    cur_ = pEnd;

    if (!advance(RPAREN))
      return parseError("Expecting )");

    builder.AppendInt(field, val);
    return Status::OK();
  }

  // NUMBERLONG :
  //   NumberLong( <number> )
  Status parseNumberLong(Slice field, BSONObjBuilder& builder) {
    if (!advance(LPAREN))
      return parseError("Expecting (");

    char *pEnd;
    int64_t val;
    int err_num;

    errno = 0;
    val = strtoll(cur_, &pEnd, 10);
    err_num = errno;

    if (pEnd == nullptr)
      return parseError("NumberLong: Invalid conversion from string to integer");

    if (err_num == ERANGE)
      return parseError("NumberLong: Value cannot fit in int64");

    cur_ = pEnd;

    if (!advance(RPAREN))
      return parseError("Expecting )");

    builder.AppendLong(field, val);
    return Status::OK();
  }

  //
  // DATETIME :
  //   Datetime( <64 bit unsigned integer for seconds since epoch> )
  //
  Status parseDatetime(Slice field, BSONObjBuilder &builder) {
    if (!advance(LPAREN))
      return parseError("Expecting (");

    char *pEnd;
    int64_t tm;
    int err_num;

    errno = 0;
    tm = strtoll(cur_, &pEnd, 10);
    err_num = errno;

    if (pEnd == nullptr)
      return parseError("Datetime: Invalid conversion from string to integer");

    if (err_num == ERANGE)
      return parseError("Datetime: Value cannot fit in int64");

    if (tm < 0)
      return parseError("Datetime: Negative integer");

    cur_ = pEnd;

    if (!advance(RPAREN))
      return parseError("Expecting )");

    builder.AppendDatetime(field, UnixTimestamp(tm));
    return Status::OK();
  }

  // @return FailedToParse status with the given message and some
  // additional context information.
  Status parseError(Slice msg) {
    std::ostringstream oss;
    oss << msg;
    oss << "\noffset:";
    oss << offset();
    oss << "\nof:";
    oss << cur_;
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

}  // namespace bson