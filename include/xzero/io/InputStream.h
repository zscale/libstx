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
