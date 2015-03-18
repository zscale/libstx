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

namespace xzero {

class IPAddress;

/**
 * Simple UDP client.
 */
class UdpClient {
 public:
  UdpClient(const IPAddress& ip, int port);
  ~UdpClient();

  /**
   * Receives the underlying system handle for given UDP communication.
   *
   * Use this for Event registration in Scheduler API.
   */
  int handle() const noexcept;

  size_t send(const BufferRef& message);
  size_t receive(Buffer* message);

 private:
  int socket_;
  int addressFamily_;
  void* sockAddr_;
  int sockAddrLen_;
};

inline int UdpClient::handle() const noexcept {
  return socket_;
}

} // namespace xzero
