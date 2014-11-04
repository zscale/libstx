// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <cstdint>

namespace xzero {

class XZERO_API OutputStream {
 public:
  virtual OutputStream() {}

  virtual void write(const char* buf, size_t size) = 0;
  virtual void write(const std::string& data);
  virtual void printf(const char* fmt, ...);
};

class XZERO_API FileOutputStream : public OutputStream {
 public:
  FileOutputStream(int fd);
  ~FileOutputStream();

  int handle() const noexcept { return fd_; }

 private:
  int fd_;
};

class XZERO_API BufferOutputStream : public OutputStream {
 public:
  BufferOutputStream(Buffer* sink);

 private:
  Buffer* sink_;
};

} // namespace xzero


















