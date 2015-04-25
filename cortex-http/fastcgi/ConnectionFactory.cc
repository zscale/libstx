// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-http/fastcgi/ConnectionFactory.h>
#include <cortex-http/fastcgi/Connection.h>
#include <cortex-base/net/Connector.h>
#include <cortex-base/WallClock.h>

namespace cortex {
namespace http {
namespace fastcgi {

ConnectionFactory::ConnectionFactory()
    : ConnectionFactory(WallClock::system(),
                        4096,
                        4 * 1024 * 1024) {
}

ConnectionFactory::ConnectionFactory(
    WallClock* clock,
    size_t maxRequestUriLength,
    size_t maxRequestBodyLength)
    : HttpConnectionFactory("fastcgi",
                            clock,
                            maxRequestUriLength,
                            maxRequestBodyLength) {
  setInputBufferSize(16 * 1024);
}

ConnectionFactory::~ConnectionFactory() {
}

cortex::Connection* ConnectionFactory::create(
    Connector* connector,
    EndPoint* endpoint) {
  return configure(new Connection(endpoint,
                                  connector->executor(),
                                  handler(),
                                  dateGenerator(),
                                  outputCompressor(),
                                  maxRequestUriLength(),
                                  maxRequestBodyLength()),
                   connector);
}

} // namespace fastcgi
} // namespace http
} // namespace cortex
