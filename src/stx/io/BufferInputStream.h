/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_io_BufferInputStream_h
#define _libstx_io_BufferInputStream_h
#include "stx/io/inputstream.h"

namespace stx {

class BufferInputStream : public RewindableInputStream {
public:

  /**
   * Create a new InputStream from the provided buffer
   *
   * @param string the input string
   */
  static std::unique_ptr<BufferInputStream> fromBuffer(const Buffer* buffer);

  /**
   * Create a new InputStream from the provided string
   *
   * @param string the input string
   */
  BufferInputStream(const Buffer* buffer);

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
  const Buffer* buf_;
  size_t cur_;
};

} // namespace stx

#endif
