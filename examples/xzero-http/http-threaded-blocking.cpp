// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/executor/ThreadedExecutor.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>
#include <unistd.h>

int main() {
  xzero::ThreadedExecutor threadedExecutor;
  xzero::Server server;
  xzero::WallClock* clock = xzero::WallClock::monotonic();
  bool shutdown = false;

  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &threadedExecutor, nullptr, nullptr, clock,
      xzero::TimeSpan::fromSeconds(30),
      xzero::TimeSpan::Zero,
      &xzero::logAndPass,
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  inet->setBlocking(true);

  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  http->setHandler([](xzero::HttpRequest* request, xzero::HttpResponse* response) {
    xzero::BufferRef body = "Hello, World\n";

    response->setStatus(xzero::HttpStatus::Ok);
    response->setContentLength(body.size());

    response->output()->write(body, std::bind(&xzero::HttpResponse::completed,
                                              response));
  });

  server.start();

  while (!shutdown)
    sleep(1);

  server.stop();

  return 0;
}
