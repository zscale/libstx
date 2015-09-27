/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stx/io/StringOutputStream.h>

namespace stx {

std::unique_ptr<StringOutputStream> StringOutputStream::fromString(
    std::string* string) {
  return std::unique_ptr<StringOutputStream>(new StringOutputStream(string));
}

StringOutputStream::StringOutputStream(std::string* string)
    : str_(string) {
}

size_t StringOutputStream::write(const char* data, size_t size) {
  *str_ += std::string(data, size);
  return size;
}

} // namespace stx
