// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/net/UdpEndPoint.h>
#include <xzero-base/net/UdpConnector.h>
#include <xzero-base/net/IPAddress.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/logging.h>

namespace xzero {

UdpEndPoint::UdpEndPoint(
    UdpConnector* connector,
    Buffer&& msg,
    struct sockaddr* remoteSock,
    int remoteSockLen)
    : DatagramEndPoint(connector, std::move(msg)),
      remoteSock_((char*) remoteSock, remoteSockLen) {
  logTrace("UdpEndPoint", "ctor");
}

UdpEndPoint::~UdpEndPoint() {
  logTrace("UdpEndPoint", "dtor");
  //static_cast<UdpConnector*>(connector())->release(this);
}

size_t UdpEndPoint::send(const BufferRef& response) {
  logTrace("UdpEndPoint", "send(): %zu bytes", response.size());

  const int flags = 0;
  ssize_t n;

  do {
    n = sendto(
        static_cast<UdpConnector*>(connector())->handle(),
        response.data(),
        response.size(),
        flags,
        (struct sockaddr*) remoteSock_.data(),
        remoteSock_.size());
  } while (n < 0 && errno == EINTR);

  if (n < 0)
    RAISE_ERRNO(errno);

  return n;
}


} // namespace xzero
