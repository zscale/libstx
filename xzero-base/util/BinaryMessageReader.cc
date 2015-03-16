// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/util/BinaryMessageReader.h>
#include <xzero-base/RuntimeError.h>

namespace xzero {
namespace util {

BinaryMessageReader::BinaryMessageReader(
    void const* buf,
    size_t buf_len) :
    ptr_(buf),
    size_(buf_len),
    pos_(0) {}

uint16_t const* BinaryMessageReader::readUInt16() {
  return static_cast<uint16_t const*>(read(sizeof(uint16_t)));
}

uint32_t const* BinaryMessageReader::readUInt32() {
  return static_cast<uint32_t const*>(read(sizeof(uint32_t)));
}

uint64_t const* BinaryMessageReader::readUInt64() {
  return static_cast<uint64_t const*>(read(sizeof(uint64_t)));
}

void const* BinaryMessageReader::read(size_t size) {
  return static_cast<void const*>(readString(size));
}

char const* BinaryMessageReader::readString(size_t size) {
  if ((pos_ + size) > size_) {
    RAISE_FAKE(kBufferOverflowError, "requested read exceeds message bounds");
  }

  auto ptr = static_cast<char const*>(ptr_) + pos_;
  pos_ += size;
  return ptr;
}

void BinaryMessageReader::rewind() {
  pos_ = 0;
}

void BinaryMessageReader::seekTo(size_t pos) {
  if (pos > size_) {
    RAISE_FAKE(kBufferOverflowError, "requested position exceeds message bounds");
  }

  pos_ = pos;
}

size_t BinaryMessageReader::remaining() const {
  return size_ - pos_;
}


} // namespace util
} // namespace xzero
