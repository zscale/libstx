// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero/net/EndPoint.h>

namespace xzero {

class SslConnector;

class SslEndPoint : public EndPoint {
 public:
  SslEndPoint(SslConnector* connector, ...);
  ~SslEndPoint();

  bool isOpen() const override;
  void close() override;
  size_t fill(Buffer* sink) override;
  size_t flush(const BufferRef& source) override;
  size_t flush(int fd, off_t offset, size_t size) override;
  void wantFill() override;
  void wantFlush() override;
  TimeSpan idleTimeout() override;
  void setIdleTimeout(TimeSpan timeout) override;
  bool isBlocking() const override;
  void setBlocking(bool enable) override;
  bool isCorking() const override;
  void setCorking(bool enable) override;
  std::string toString() const override;

 private:
  void* bio_;
};

} // namespace xzero
