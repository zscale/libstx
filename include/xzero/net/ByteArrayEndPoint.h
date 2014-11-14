// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/sysconfig.h>
#include <xzero/Buffer.h>
#include <xzero/net/EndPoint.h>

namespace xzero {

class LocalConnector;

/**
 * Buffer-based dual-channel EndPoint.
 *
 * @see LocalEndPoint
 * @see InetEndPoint
 */
class XZERO_API ByteArrayEndPoint : public EndPoint {
 public:
  explicit ByteArrayEndPoint(LocalConnector* connector);
  ~ByteArrayEndPoint();

  /**
   * Assigns an input to this endpoint.
   *
   * @param input a movable buffer object.
   */
  void setInput(Buffer&& input);

  /**
   * Assigns an input to this endpoint.
   *
   * @param input a C-string.
   */
  void setInput(const char* input);

  /**
   * Retrieves a reference to the input buffer.
   */
  const Buffer& input() const;

  /**
   * Retrieves a reference to the output buffer.
   */
  const Buffer& output() const;

  /**
   * Tests whether this InetEndPoint is currently handling and I/O notification.
   * @see wantFill()
   * @see wantFlush()
   */
  bool isBusy() const XZERO_NOEXCEPT { return isBusy_; }

  // overrides
  void close() override;
  bool isOpen() const override;
  std::string toString() const override;
  size_t fill(Buffer*) override;
  size_t flush(const BufferRef&) override;
  size_t flush(int fd, off_t offset, size_t size) override;
  void wantFill() override;
  void wantFlush(bool enable) override;
  TimeSpan idleTimeout() override;
  void setIdleTimeout(TimeSpan timeout) override;
  bool isBlocking() const override;
  void setBlocking(bool enable) override;
  bool isCorking() const override;
  void setCorking(bool enable) override;

 private:
  LocalConnector* connector_;
  int isBusy_;
  Buffer input_;
  size_t readPos_;
  Buffer output_;
  bool closed_;
};

} // namespace xzero
