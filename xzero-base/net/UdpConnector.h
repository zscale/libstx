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

class XZERO_API UdpConnector : public DatagramConnector {
 public:
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

class StatsServer {
 public:
  DatagramConnector* dgramConnector_;
};


} // namespace xzero

