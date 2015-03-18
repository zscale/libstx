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

namespace xzero {

UdpEndPoint::UdpEndPoint(
    UdpConnector* connector,
    Buffer&& msg,
    const IPAddress& remoteIP)
    : DatagramEndPoint(connector, std::move(msg)) {
}

UdpEndPoint::~UdpEndPoint() {
}

void UdpEndPoint::reply(const BufferRef& response) {
  if (eof_)
    RAISE(IllegalStateError);

  const int flags = 0;

  send(static_cast<UdpConnector*>(connector())->handle(),
       response.data(),
       response.size(),
       flags);
}


} // namespace xzero
