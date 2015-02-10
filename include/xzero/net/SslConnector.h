// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero/net/InetConnector.h>
#include <xzero/net/SslEndPoint.h>

namespace xzero {

class SslConnector : public InetConnector {
 public:
  SslConnector();
  ~SslConnector();

  void start() override;
  bool isStarted() const XZERO_NOEXCEPT override;
  void stop() override;
  std::list<RefPtr<EndPoint>> connectedEndPoints() override;
};

} // namespace xzero


