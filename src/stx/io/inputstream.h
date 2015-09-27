/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_INPUTSTREAM_H
#define _libstx_INPUTSTREAM_H

// #if (!defined(_libstx_io_BufferInputStream_h) && \
//      !defined(_libstx_io_FileInputStream_h) && \
//      !defined(_libstx_io_MemoryInputStream_h) && \
//      !defined(_libstx_io_StringInputStream_h))
// #warning "Do include <stx/io/{Buffer,File,Memory,String}InputStream.h> directly."
// #endif

#include <memory>
#include <string>

namespace stx {

class Buffer;

class InputStream {
public:
  /**
   * Get the stdin input stream
   */
  static std::unique_ptr<InputStream> getStdin();

  InputStream(const std::string& filename = "<anonymous input stream>");
  InputStream(const InputStream& other) = delete;
  InputStream& operator=(const InputStream& other) = delete;
  virtual ~InputStream() {}

  /**
   * Read the next byte from the input stream. Returns true if the next byte was
   * read and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  virtual bool readNextByte(char* target) = 0;

  /**
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  virtual bool eof() = 0;

  /**
   * Read N bytes from the stream and copy the data into the provided string.
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   * @param n_bytes the number of bytes to read
   */
  virtual size_t readNextBytes(std::string* target, size_t n_bytes);

  /**
   * Read N bytes from the stream and copy the data into the provided buffer
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   * @param n_bytes the number of bytes to read
   */
  virtual size_t readNextBytes(Buffer* target, size_t n_bytes);

  /**
   * Read N bytes from the stream and copy the data into the provided buffer
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   * @param n_bytes the number of bytes to read
   */
  virtual size_t readNextBytes(void* target, size_t n_bytes);

  /**
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  virtual size_t skipNextBytes(size_t n_bytes) = 0;

  /**
   * Read from the stream until EOF and copy the data into the provided string.
   * Returns the number of bytes read.
   *
   * WARNING: this method might never return if the input stream does not have
   * an EOF!
   *
   * @param target the string to copy the data into
   */
  virtual size_t readUntilEOF(std::string* target);

  /**
   * Read from the stream until the next '\n' character and copy the data into
   * the provided string. Returns the number of bytes read.
   *
   * WARNING: this method might never return if the input stream does not
   * contain '\n' characters and does not have an EOF!
   *
   * Returns false if the EOF was reached and true if it wasn't.
   *
   * @param target the string to copy the data into
   */
  virtual bool readLine(std::string* target);

  /**
   * Reads a uint8 from the stream. Throws an exception on error
   */
  virtual uint8_t readUInt8();

  /**
   * Reads a uint16 from the stream. Throws an exception on error
   */
  virtual uint16_t readUInt16();

  /**
   * Reads a uint32 from the stream. Throws an exception on error
   */
  virtual uint32_t readUInt32();

  /**
   * Reads a uint64 from the stream. Throws an exception on error
   */
  virtual uint64_t readUInt64();

  /**
   * Reads a LEB128 encoded uint64 from the stream. Throws an exception on error
   */
  virtual uint64_t readVarUInt();

  /**
   * Reads a string from the stream. Throws an exception on error
   */
  virtual std::string readString(size_t size);

  /**
   * Reads a LEB128 prefix-length-encoded string from the stream. Throws an
   * exception on error
   */
  virtual std::string readLenencString();

  /**
   * Reads a IEEE754 encoded double from the stream. Throws an exception on
   * error
   */
  virtual double readDouble();

  /**
   * Return the input stream filename
   */
  const std::string& getFileName() const;

  /**
   * Set the input stream filename
   */
  void setFileName(const std::string& filename);

private:
  std::string filename_;
};

class RewindableInputStream : public InputStream {
public:
  /**
   * Rewind the input stream
   */
  virtual void rewind() = 0;
};

} // namespace stx

// FIXME: this is just a quick hack to not break external deps
// The app should include those headers directly instead.
#include <stx/io/StringInputStream.h>
#include <stx/io/BufferInputStream.h>
#include <stx/io/MemoryInputStream.h>
#include <stx/io/FileInputStream.h>

#endif
