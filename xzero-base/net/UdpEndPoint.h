// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/net/DatagramEndPoint.h>

namespace xzero {

class UdpConnector;
class IPAddress;

class XZERO_API UdpEndPoint : public DatagramEndPoint {
 public:
  UdpEndPoint(UdpConnector* connector, Buffer&& msg, const IPAddress& remoteIP);
  ~UdpEndPoint();

  void reply(const BufferRef& response) override;

 private:
  //! indicator if already replied or not.
  bool eof_;
};

} // namespace xzero

