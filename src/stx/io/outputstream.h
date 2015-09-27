/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_OUTPUTSTREAM_H
#define _libstx_OUTPUTSTREAM_H

// #if (!defined(_libstx_io_BufferOutputStream_h) && \
//      !defined(_libstx_io_FileOutputStream_h) && \
//      !defined(_libstx_io_StringOutputStream_h))
// #warning "Do include <stx/io/{Buffer,File,String}OutputStream.h> directly."
// #endif

#include <memory>
#include <mutex>

namespace stx {

class Buffer;

class OutputStream {
public:
  OutputStream() {}
  OutputStream(const OutputStream& other) = delete;
  OutputStream& operator=(const OutputStream& other) = delete;
  virtual ~OutputStream() {}

  /**
   * Get the stdout output stream
   */
  static std::unique_ptr<OutputStream> getStdout();

  /**
   * Get the stderr output stream
   */
  static std::unique_ptr<OutputStream> getStderr();

  /**
   * Write the next n bytes to the output stream. This may raise an exception.
   * Returns the number of bytes that have been written.
   *
   * @param data a pointer to the data to be written
   * @param size then number of bytes to be written
   */
  virtual size_t write(const char* data, size_t size) = 0;
  virtual size_t write(const std::string& data);
  virtual size_t write(const Buffer& buf);
  virtual size_t printf(const char* format, ...);

  /**
   * Writes a plain uint8 to the stream.
   */
  void appendUInt8(uint8_t value);

  /**
   * Writes a plain uint16 to the stream.
   */
  void appendUInt16(uint16_t value);

  /**
   * Writes a plain uint32 to the stream.
   */
  void appendUInt32(uint32_t value);

  /**
   * Writes a plain uint64 to the stream.
   */
  void appendUInt64(uint64_t value);

  /**
   * Writes a LEB128 encoded uint64 to the stream.
   */
  void appendVarUInt(uint64_t value);

  /**
   * Writes a IEEE754 encoded double to the stream.
   */
  void appendDouble(double value);

  /**
   * Writes a string to the stream.
   */
  void appendString(const std::string& string);

  /**
   * Writes a LEB128 prefix-length-encoded string to the stream.
   */
  void appendLenencString(const std::string& string);
  void appendLenencString(const void* data, size_t size);

  /**
   * Publicly accessible mutex that may be used to synchronize writes to this
   * output stream
   */
  mutable std::mutex mutex;
};

} // namespace stx

// FIXME: this is just a quick hack to not break external deps
// The app should include those headers directly instead.
#include <stx/io/BufferOutputStream.h>
#include <stx/io/FileOutputStream.h>
#include <stx/io/StringOutputStream.h>

#endif
