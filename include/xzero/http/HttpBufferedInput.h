// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/http/HttpInput.h>
#include <xzero/Buffer.h>

namespace xzero {

class HttpConnection;

/**
 * HTTP/1 message body consumer.
 */
class HttpBufferedInput : public xzero::HttpInput {
 public:
  HttpBufferedInput();
  ~HttpBufferedInput();

  int read(Buffer* result) override;
  size_t readLine(Buffer* result) override;
  void onContent(const BufferRef& chunk) override;
  bool empty() const noexcept override;

  void recycle() override;

 private:
  Buffer content_;
  size_t offset_;
};

}  // namespace xzero
