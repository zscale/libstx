// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-http/http1/Http1ConnectionFactory.h>
#include <cortex-http/http1/HttpConnection.h>
#include <cortex-base/net/Connector.h>
#include <cortex-base/WallClock.h>

namespace cortex {
namespace http {
namespace http1 {

Http1ConnectionFactory::Http1ConnectionFactory()
    : Http1ConnectionFactory(WallClock::system(),
                             4096,
                             4 * 1024 * 1024,
                             100,
                             TimeSpan::fromSeconds(8)) {
}

Http1ConnectionFactory::Http1ConnectionFactory(
    WallClock* clock,
    size_t maxRequestUriLength,
    size_t maxRequestBodyLength,
    size_t maxRequestCount,
    TimeSpan maxKeepAlive)
    : HttpConnectionFactory("http/1.1", clock, maxRequestUriLength,
                            maxRequestBodyLength),
      maxRequestCount_(maxRequestCount),
      maxKeepAlive_(maxKeepAlive) {
  setInputBufferSize(16 * 1024);
}

Http1ConnectionFactory::~Http1ConnectionFactory() {
}

Connection* Http1ConnectionFactory::create(Connector* connector,
                                           EndPoint* endpoint) {
  return configure(new http1::HttpConnection(endpoint,
                                             connector->executor(),
                                             handler(),
                                             dateGenerator(),
                                             outputCompressor(),
                                             maxRequestUriLength(),
                                             maxRequestBodyLength(),
                                             maxRequestCount(),
                                             maxKeepAlive()),
                   connector);
}

}  // namespace http1
}  // namespace http
}  // namespace cortex
