// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/net/DatagramEndPoint.h>
#include <vector>

namespace xzero {

class LocalDatagramConnector;

/**
 * In-memory based datagram endpoint.
 *
 * @see LocalDatagramConnector.
 */
class XZERO_API LocalDatagramEndPoint : public DatagramEndPoint {
 public:
  LocalDatagramEndPoint(LocalDatagramConnector* connector, Buffer&& msg);
  ~LocalDatagramEndPoint();

  size_t send(const BufferRef& response) override;

  const std::vector<Buffer>& responses() const noexcept { return responses_; }

 private:
  std::vector<Buffer> responses_;
};

} // namespace xzero
