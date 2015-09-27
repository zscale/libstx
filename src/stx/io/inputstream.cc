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
#include <stx/ieee754.h>

namespace stx {

std::unique_ptr<InputStream> InputStream::getStdin() {
  auto stdin_stream = new FileInputStream(0, false);
  return std::unique_ptr<InputStream>(stdin_stream);
}

InputStream::InputStream(
    const std::string& filename /* = "<anonymous input stream>" */) :
    filename_(filename) {}

const std::string& InputStream::getFileName() const {
  return filename_;
}

void InputStream::setFileName(const std::string& filename) {
  filename_ = filename;
}

// FIXPAUL: optimize?
size_t InputStream::readNextBytes(std::string* target, size_t n_bytes) {
  char byte;
  size_t length;

  for (length = 0; length < n_bytes && readNextByte(&byte); ++length) {
    *target += byte;
  }

  return length;
}

// FIXPAUL: optimize?
size_t InputStream::readNextBytes(Buffer* target, size_t n_bytes) {
  char byte;
  size_t length;

  target->reserve(n_bytes);

  for (length = 0; length < n_bytes && readNextByte(&byte); ++length) {
    target->append(&byte, sizeof(byte));
  }

  return length;
}

size_t InputStream::readNextBytes(void* target, size_t n_bytes) {
  size_t length = 0;

  while (length < n_bytes && readNextByte(((char*) target) + length)) {
    ++length;
  }

  return length;
}

// FIXPAUL: optimize?
size_t InputStream::readUntilEOF(std::string* target) {
  char byte;
  size_t length;

  for (length = 0; readNextByte(&byte); ++length) {
    *target += byte;
  }

  return length;
}

bool InputStream::readLine(std::string* target) {
  char byte;
  size_t length;

  for (length = 0; readNextByte(&byte); ++length) {
    *target += byte;

    if (byte == '\n') {
      return true;
    }
  }

  return length > 0;
}

uint8_t InputStream::readUInt8() {
  uint8_t val;
  if (readNextBytes(&val, sizeof(uint8_t)) != sizeof(uint8_t)) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return val;
}

uint16_t InputStream::readUInt16() {
  uint16_t val;
  if (readNextBytes(&val, sizeof(uint16_t)) != sizeof(uint16_t)) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return val;
}

uint32_t InputStream::readUInt32() {
  uint32_t val;
  if (readNextBytes(&val, sizeof(uint32_t)) != sizeof(uint32_t)) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return val;
}

uint64_t InputStream::readUInt64() {
  uint64_t val;
  if (readNextBytes(&val, sizeof(uint64_t)) != sizeof(uint64_t)) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return val;
}

uint64_t InputStream::readVarUInt() {
  uint64_t val = 0;

  for (int i = 0; ; ++i) {
    unsigned char b;
    if (!readNextByte((char*) &b)) {
      RAISE(kEOFError, "unexpected end of stream");
    }

    val |= (b & 0x7fULL) << (7 * i);

    if (!(b & 0x80U)) {
      break;
    }
  }

  return val;
}

String InputStream::readString(size_t size) {
  String val;
  if (readNextBytes(&val, size) != size) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return val;
}

String InputStream::readLenencString() {
  auto size = readVarUInt();

  String val;
  if (readNextBytes(&val, size) != size) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return val;
}

double InputStream::readDouble() {
  uint64_t val;
  if (readNextBytes(&val, sizeof(uint64_t)) != sizeof(uint64_t)) {
    RAISE(kEOFError, "unexpected end of stream");
  }

  return IEEE754::fromBytes(val);
}

} // namespace stx
