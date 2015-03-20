// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/executor/Scheduler.h>
#include <xzero-base/net/DatagramConnector.h>
#include <xzero-base/net/LocalDatagramEndPoint.h>
#include <xzero-base/net/IPAddress.h>
#include <functional>

namespace xzero {

/**
 * Datagram Connector for in-memory messages.
 *
 * @see DatagramConnector, DatagramEndPoint
 * @see UdpConnector, UdpEndPoint
 */
class XZERO_API LocalDatagramConnector : public DatagramConnector {
 public:
  /**
   * Initializes the UDP connector.
   *
   * @param name Human readable name for the given connector (such as "ntp").
   * @param handler Callback handler to be invoked on every incoming message.
   * @param executor Executor service to be used for invoking the handler.
   */
  LocalDatagramConnector(
      const std::string& name,
      DatagramHandler handler,
      Executor* executor);

  ~LocalDatagramConnector();

  void start() override;
  bool isStarted() const override;
  void stop() override;

  RefPtr<LocalDatagramEndPoint> send(const BufferRef& message);
  RefPtr<LocalDatagramEndPoint> send(Buffer&& message);

 private:
  void runQueue();

 private:
  bool isStarted_;
  std::deque<RefPtr<LocalDatagramEndPoint>> pending_;
};

} // namespace xzero
