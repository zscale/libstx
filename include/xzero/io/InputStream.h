// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>

namespace xzero {

class Buffer;
class OutputStream;

class InputStream {
 public:
  virtual ~InputStream() {}

  virtual size_t read(Buffer* target, size_t n) = 0;
  virtual size_t transferTo(OutputStream* target) = 0;
};

class BufferInputStream {
 public:
  explicit BufferInputStream(Buffer* source);

  void rewind();

  size_t read(Buffer* target, size_t n) override;
  size_t transferTo(OutputStream* target) override;

 private:
  Buffer* source_;
  size_t offse_;
};

} // namespace xzero
