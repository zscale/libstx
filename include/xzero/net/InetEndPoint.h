// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/io/Selectable.h>
#include <xzero/IdleTimeout.h>
#include <xzero/net/EndPoint.h>
#include <xzero/net/IPAddress.h>

namespace xzero {

class InetConnector;
class SelectionKey;

/**
 * TCP/IP endpoint, as created by the InetConnector.
 */
class XZERO_API InetEndPoint : public EndPoint, public Selectable {
 public:
  InetEndPoint(int socket, InetConnector* connector);
  ~InetEndPoint();

  /**
   * Retrieves remote address + port.
   */
  std::pair<IPAddress, int> remoteAddress() const;

  /**
   * Retrieves local address + port.
   */
  std::pair<IPAddress, int> localAddress() const;

  /**
   * Tests whether this InetEndPoint is currently handling and I/O notification.
   * @see wantFill()
   * @see wantFlush()
   */
  bool isBusy() const noexcept { return isBusy_; }

  // EndPoint overrides
  bool isOpen() const noexcept override;
  void close() override;
  bool isBlocking() const override;
  void setBlocking(bool enable) override;
  bool isCorking() const override;
  void setCorking(bool enable) override;
  std::string toString() const override;
  size_t fill(Buffer* result) override;
  size_t flush(const BufferRef& source) override;
  size_t flush(int fd, off_t offset, size_t size) override;
  void wantFill() override;
  void wantFlush(bool enable) override;
  TimeSpan idleTimeout() override;
  void setIdleTimeout(TimeSpan timeout) override;

  // Selectable overrides
  int handle() const noexcept override;
  Selector* selector() const noexcept override;
  void onSelectable() noexcept override;

 private:
  void fillable();
  void flushable();
  void onTimeout();

 private:
  InetConnector* connector_;
  IdleTimeout idleTimeout_;
  int handle_;
  std::unique_ptr<SelectionKey> selectionKey_;
  bool isCorking_;
  int isBusy_;

  /**
   * Helper class to aid easy scoped business refcounting to ease exception
   * handling.
   */
  class BusyGuard {
   public:
    BusyGuard(InetEndPoint* ep) noexcept : ep_(ep) { ep->isBusy_++; }
    ~BusyGuard() noexcept { ep_->isBusy_--; }

   private:
    InetEndPoint* ep_;
  };
};

} // namespace xzero
