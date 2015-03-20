// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/executor/Scheduler.h>
#include <xzero-base/net/DatagramConnector.h>
#include <xzero-base/net/IPAddress.h>
#include <functional>

namespace xzero {

class Scheduler;

/**
 * Datagram Connector for UDP protocol.
 *
 * @see DatagramConnector, DatagramEndPoint
 */
class XZERO_API UdpConnector : public DatagramConnector {
 public:
  /**
   * Initializes the UDP connector.
   *
   * @param name Human readable name for the given connector (such as "ntp").
   * @param handler Callback handler to be invoked on every incoming message.
   * @param executor Executor service to be used for invoking the handler.
   * @param scheduler Scheduler service to be used for I/O notifications.
   * @param ipaddress IP address to bind the connector to.
   * @param port UDP port number to bind the connector to.
   * @param reuseAddr Whether or not to enable @c SO_REUSEADDR.
   * @param reusePort Whether or not to enable @c SO_REUSEPORT.
   */
  UdpConnector(
      const std::string& name,
      DatagramHandler handler,
      Executor* executor,
      Scheduler* scheduler,
      const IPAddress& ipaddress, int port,
      bool reuseAddr, bool reusePort);

  ~UdpConnector();

  int handle() const noexcept { return socket_; }

  void start() override;
  bool isStarted() const override;
  void stop() override;

 private:
  void open(
      const IPAddress& bind, int port,
      bool reuseAddr, bool reusePort);

  void notifyOnEvent();
  void onMessage();

 private:
  Scheduler* scheduler_;
  Scheduler::HandleRef schedulerHandle_;
  int socket_;
  int addressFamily_;
};

} // namespace xzero
