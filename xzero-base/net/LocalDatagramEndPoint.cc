// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/net/LocalDatagramEndPoint.h>
#include <xzero-base/net/LocalDatagramConnector.h>

namespace xzero {

LocalDatagramEndPoint::LocalDatagramEndPoint(
    LocalDatagramConnector* connector, Buffer&& msg)
    : DatagramEndPoint(connector, std::move(msg)) {
}

LocalDatagramEndPoint::~LocalDatagramEndPoint() {
}

size_t LocalDatagramEndPoint::send(const BufferRef& response) {
  responses_.emplace_back(response);
  return response.size();
}

} // namespace xzero
