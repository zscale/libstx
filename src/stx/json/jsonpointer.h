/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_JSON_JSONPOINTER_H
#define _STX_JSON_JSONPOINTER_H
#include <stdlib.h>
#include <string>

namespace stx {
namespace json {

class JSONPointer {
public:
  static void escape(std::string* str);

  JSONPointer();
  JSONPointer(const char* path);
  explicit JSONPointer(std::string path);

  bool operator==(const JSONPointer& other) const {
    return path_ == other.path_;
  }

  inline const std::string toString() const {
    return path_;
  }

  void push(std::string str);
  void pop();

  std::string head() const;

protected:
  std::string path_;
};

} // namespace json
} // namespace stx
#endif
