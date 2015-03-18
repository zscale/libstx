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
#include <xzero-base/RefCounted.h>
#include <functional>

namespace xzero {

class DatagramConnector;

class XZERO_API DatagramEndPoint : public RefCounted {
 public:
  DatagramEndPoint(DatagramConnector* connector, Buffer&& msg)
    : connector_(connector), message_(std::move(msg)) {}

  ~DatagramEndPoint();

  DatagramConnector* connector() const noexcept { return connector_; }

  const BufferRef& message() const { return message_; }

  virtual size_t send(const BufferRef& response) = 0;

 private:
  //! the connector that was used to receive the message
  DatagramConnector* connector_;

  //! message received
  Buffer message_;
};

} // namespace xzero
