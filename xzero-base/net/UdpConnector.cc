// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/net/UdpConnector.h>
#include <xzero-base/net/UdpEndPoint.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/executor/Scheduler.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/logging.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

namespace xzero {

UdpConnector::UdpConnector(
    const std::string& name,
    DatagramHandler handler,
    Executor* executor,
    Scheduler* scheduler,
    const IPAddress& ipaddr, int port,
    bool reuseAddr, bool reusePort)
    : DatagramConnector(name, handler, executor),
      scheduler_(scheduler),
      socket_(-1),
      addressFamily_(0) {
  open(ipaddr, port, reuseAddr, reusePort);
}

UdpConnector::~UdpConnector() {
  if (isStarted()) {
    stop();
  }
}

void UdpConnector::start() {
  notifyOnEvent();
}

bool UdpConnector::isStarted() const {
  return socket_ >= 0;
}

void UdpConnector::stop() {
  if (!isStarted())
    RAISE(IllegalStateError);

  if (schedulerHandle_)
    schedulerHandle_->cancel();

  ::close(socket_);
  socket_ = -1;
}

void UdpConnector::open(
    const IPAddress& ipaddr, int port,
    bool reuseAddr, bool reusePort) {

  socket_ = ::socket(ipaddr.family(), SOCK_DGRAM, 0);
  addressFamily_ = ipaddr.family();

  if (socket_ < 0)
    RAISE_ERRNO(errno);

  if (reusePort) {
    int rc = 1;
    if (::setsockopt(socket_, SOL_SOCKET, SO_REUSEPORT, &rc, sizeof(rc)) < 0) {
      RAISE_ERRNO(errno);
    }
  }

  if (reuseAddr) {
    int rc = 1;
    if (::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &rc, sizeof(rc)) < 0) {
      RAISE_ERRNO(errno);
    }
  }

  // bind(ipaddr, port);
  char sa[sizeof(sockaddr_in6)];
  socklen_t salen = ipaddr.size();

  switch (ipaddr.family()) {
    case IPAddress::V4:
      salen = sizeof(sockaddr_in);
      ((sockaddr_in*)sa)->sin_port = htons(port);
      ((sockaddr_in*)sa)->sin_family = AF_INET;
      memcpy(&((sockaddr_in*)sa)->sin_addr, ipaddr.data(), ipaddr.size());
      break;
    case IPAddress::V6:
      salen = sizeof(sockaddr_in6);
      ((sockaddr_in6*)sa)->sin6_port = htons(port);
      ((sockaddr_in6*)sa)->sin6_family = AF_INET6;
      memcpy(&((sockaddr_in6*)sa)->sin6_addr, ipaddr.data(), ipaddr.size());
      break;
    default:
      RAISE_ERRNO(EINVAL);
  }

  int rv = ::bind(socket_, (sockaddr*)sa, salen);
  if (rv < 0)
    RAISE_ERRNO(errno);
}

void UdpConnector::notifyOnEvent() {
  logTrace("UdpConnector", "notifyOnEvent()");
  schedulerHandle_ = scheduler_->executeOnReadable(
      socket_,
      std::bind(&UdpConnector::onMessage, this));
}

void UdpConnector::onMessage() {
  logTrace("UdpConnector", "onMessage");

  Buffer message;
  message.reserve(65535);

  sockaddr_in remoteAddr;
  socklen_t remoteSocketLen = sizeof(remoteAddr);

  notifyOnEvent();

  int n = recvfrom(
      socket_,
      message.data(),
      message.capacity(),
      0,
      (sockaddr*) &remoteAddr,
      &remoteSocketLen);

  if (n < 0)
    RAISE_ERRNO(errno);

  if (handler_) {
    logTrace("UdpConnector", "handler set");
    message.resize(n);
    RefPtr<DatagramEndPoint> client(new UdpEndPoint(
        this, std::move(message), (sockaddr*) &remoteAddr, remoteSocketLen));
    handler_(client);
  } else {
    logTrace("UdpConnector",
             "ignoring incoming message of %i bytes. No handler set.", n);
  }
}

} // namespace xzero
