// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <cortex-http/HttpChannel.h>
#include <cortex-base/sysconfig.h>
#include <list>
#include <string>

namespace cortex {
namespace http1 {

class Http1Channel : public cortex::HttpChannel {
 public:
  Http1Channel(HttpTransport* transport,
              const HttpHandler& handler,
              std::unique_ptr<HttpInput>&& input,
              size_t maxRequestUriLength,
              size_t maxRequestBodyLength,
              HttpOutputCompressor* outputCompressor);
  ~Http1Channel();

  bool isPersistent() const XZERO_NOEXCEPT { return persistent_; }
  void setPersistent(bool value) XZERO_NOEXCEPT { persistent_ = value; }

  virtual void reset();

 protected:
  bool onMessageBegin(const BufferRef& method, const BufferRef& entity,
                      HttpVersion version) override;
  bool onMessageHeader(const BufferRef& name, const BufferRef& value) override;
  bool onMessageHeaderEnd() override;
  void onProtocolError(HttpStatus code, const std::string& message) override;

 private:
  bool persistent_;
  std::list<std::string> connectionOptions_;
};

} // namespace http1
} // namespace cortex
