/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_io_BufferOutputStream_h
#define _libstx_io_BufferOutputStream_h
#include <memory>
#include <stx/io/outputstream.h>
#include <stx/buffer.h>

namespace stx {

class BufferOutputStream : public OutputStream {
public:
  /**
   * Create a new OutputStream from the provided string
   *
   * @param buf the output buffer
   */
  static std::unique_ptr<BufferOutputStream> fromBuffer(Buffer* buf);

  /**
   * Create a new OutputStream from the provided buffer
   *
   * @param buf the output buffer
   */
  BufferOutputStream(Buffer* string);

  /**
   * Write the next n bytes to the file. This may raise an exception.
   * Returns the number of bytes that have been written.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  size_t write(const char* data, size_t size) override;

protected:
  Buffer* buf_;
};

}
#endif
