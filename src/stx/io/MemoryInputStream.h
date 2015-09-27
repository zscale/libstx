/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_io_MemoryInputStream_h
#define _libstx_io_MemoryInputStream_h
#include <memory>
#include <string>
#include "stx/buffer.h"
#include "stx/io/file.h"

namespace stx {

class MemoryInputStream : public RewindableInputStream {
public:

  /**
   * Create a new InputStream from the provided string
   *
   * @param string the input string
   */
  MemoryInputStream(const void* data, size_t size);

  /**
   * Read the next byte from the file. Returns true if the next byte was read
   * and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  bool readNextByte(char* target) override;

  /**
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  size_t skipNextBytes(size_t n_bytes) override;

  /**
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  bool eof() override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

protected:
  const void* data_;
  size_t size_;
  size_t cur_;
};

}

#endif
