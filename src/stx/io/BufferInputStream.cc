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
#include <stx/io/BufferInputStream.h>
#include <stx/ieee754.h>

namespace stx {

std::unique_ptr<BufferInputStream> BufferInputStream::fromBuffer(
    const Buffer* buf) {
  return std::unique_ptr<BufferInputStream>(new BufferInputStream(buf));
}

BufferInputStream::BufferInputStream(
    const Buffer* buf) :
    buf_(buf),
    cur_(0) {
}

bool BufferInputStream::readNextByte(char* target) {
  if (cur_ < buf_->size()) {
    *target = buf_->charAt(cur_++);
    return true;
  } else {
    return false;
  }
}

size_t BufferInputStream::skipNextBytes(size_t n_bytes) {
  auto nskipped = n_bytes;
  if (cur_ + n_bytes > buf_->size()) {
    nskipped = buf_->size() - cur_;
  }

  cur_ += nskipped;
  return nskipped;
}

bool BufferInputStream::eof() {
  return cur_ >= buf_->size();
}

void BufferInputStream::rewind() {
  cur_ = 0;
}

} // namespace stx
