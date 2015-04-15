// This file is part of the "libcortex" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libcortex is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cortex-base/Api.h>
#include <cortex-base/sysconfig.h>
#include <cortex-base/IdleTimeout.h>
#include <cortex-base/net/EndPoint.h>
#include <cortex-base/net/IPAddress.h>
#include <atomic>

namespace cortex {

class InetConnector;

/**
 * TCP/IP endpoint, as created by the InetConnector.
 */
class CORTEX_API InetEndPoint : public EndPoint {
 public:
  InetEndPoint(int socket, InetConnector* connector, Scheduler* scheduler);
  ~InetEndPoint();

  int handle() const noexcept { return handle_; }

  /**
   * Retrieves remote address + port.
   */
  std::pair<IPAddress, int> remoteAddress() const;

  /**
   * Retrieves local address + port.
   */
  std::pair<IPAddress, int> localAddress() const;

  // EndPoint overrides
  bool isOpen() const CORTEX_NOEXCEPT override;
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
  void wantFlush() override;
  TimeSpan readTimeout() override;
  TimeSpan writeTimeout() override;
  void setReadTimeout(TimeSpan timeout) override;
  void setWriteTimeout(TimeSpan timeout) override;

 private:
  void onReadable() CORTEX_NOEXCEPT;
  void onWritable() CORTEX_NOEXCEPT;

  void fillable();
  void flushable();
  void onTimeout();

 private:
  InetConnector* connector_;
  Scheduler* scheduler_;
  TimeSpan readTimeout_;
  TimeSpan writeTimeout_;
  IdleTimeout idleTimeout_;
  Scheduler::HandleRef io_;
  int handle_;
  bool isCorking_;
};

} // namespace cortex
