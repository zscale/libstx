/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stx/buffer.h>
#include <stx/exception.h>
#include <stx/io/inputstream.h>
#include <stx/io/StringInputStream.h>
#include <stx/ieee754.h>

namespace stx {

std::unique_ptr<StringInputStream> StringInputStream::fromString(
    const std::string& string) {
  return std::unique_ptr<StringInputStream>(new StringInputStream(string));
}

StringInputStream::StringInputStream(
    const std::string& string) :
    str_(string),
    cur_(0) {
}

bool StringInputStream::readNextByte(char* target) {
  if (cur_ < str_.size()) {
    *target = str_[cur_++];
    return true;
  } else {
    return false;
  }
}

size_t StringInputStream::skipNextBytes(size_t n_bytes) {
  auto nskipped = n_bytes;
  if (cur_ + n_bytes > str_.size()) {
    nskipped = str_.size() - cur_;
  }

  cur_ += nskipped;
  return nskipped;
}

bool StringInputStream::eof() {
  return cur_ >= str_.size();
}

void StringInputStream::rewind() {
  cur_ = 0;
}

} // namespace stx
