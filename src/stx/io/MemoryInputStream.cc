/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stx/io/inputstream.h>
#include <stx/io/MemoryInputStream.h>

namespace stx {

MemoryInputStream::MemoryInputStream(
    const void* data,
    size_t size) :
    data_(data),
    size_(size),
    cur_(0) {}

bool MemoryInputStream::readNextByte(char* target) {
  if (cur_ < size_) {
    *target = *(((char*) data_) + cur_++);
    return true;
  } else {
    return false;
  }
}

size_t MemoryInputStream::skipNextBytes(size_t n_bytes) {
  auto nskipped = n_bytes;
  if (cur_ + n_bytes > size_) {
    nskipped = size_ - cur_;
  }

  cur_ += nskipped;
  return nskipped;
}

bool MemoryInputStream::eof() {
  return cur_ >= size_;
}

void MemoryInputStream::rewind() {
  cur_ = 0;
}

} // namespace stx

