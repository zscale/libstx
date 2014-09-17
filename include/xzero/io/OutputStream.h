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


















