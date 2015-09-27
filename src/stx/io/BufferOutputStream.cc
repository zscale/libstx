/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stx/io/BufferOutputStream.h>

namespace stx {

std::unique_ptr<BufferOutputStream> BufferOutputStream::fromBuffer(Buffer* buf) {
  return std::unique_ptr<BufferOutputStream>(new BufferOutputStream(buf));
}

BufferOutputStream::BufferOutputStream(Buffer* buf)
    : buf_(buf) {
}

size_t BufferOutputStream::write(const char* data, size_t size) {
  buf_->append(data, size);
  return size;
}

} // namespace stx
