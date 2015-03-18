// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/net/UdpClient.h>
#include <xzero-base/net/IPAddress.h>
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

UdpClient::UdpClient(const IPAddress& ipaddr, int port)
    : socket_(-1),
      addressFamily_(0),
      sockAddr_(nullptr),
      sockAddrLen_(0)
{
  switch (ipaddr.family()) {
    case IPAddress::V4:
      sockAddrLen_ = sizeof(sockaddr_in);
      break;
    case IPAddress::V6:
      sockAddrLen_ = sizeof(sockaddr_in6);
      break;
    default:
      RAISE(IllegalStateError);
  }

  sockAddr_ = malloc(sockAddrLen_);
  if (!sockAddr_)
    RAISE(MallocError);

  addressFamily_ = ipaddr.family();

  socket_ = ::socket(ipaddr.family(), SOCK_DGRAM, 0);
}

UdpClient::~UdpClient() {
  if (socket_ >= 0) {
    ::close(socket_);
  }
  free(sockAddr_);
}

size_t UdpClient::send(const BufferRef& message) {
  const int flags = 0;
  ssize_t n;

  do {
    n = sendto(
        socket_,
        message.data(),
        message.size(),
        flags,
        (sockaddr*) sockAddr_,
        sockAddrLen_);
  } while (n < 0 && errno == EINTR);

  if (n < 0)
    RAISE_ERRNO(errno);

  return n;
}

size_t UdpClient::receive(Buffer* message) {
  int n;

  message->reserve(65536);
  socklen_t socklen = (socklen_t) sockAddrLen_;

  do {
    n = recvfrom(
        socket_,
        message->data(),
        message->capacity(),
        0,
        (sockaddr*) sockAddr_,
        &socklen);
  } while (n < 0 && errno == EINTR);

  if (n < 0)
    RAISE_ERRNO(errno);

  return static_cast<size_t>(n);
}

} // namespace xzero
