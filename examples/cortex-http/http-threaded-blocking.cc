// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-base/executor/ThreadedExecutor.h>
#include <cortex-base/net/Server.h>
#include <cortex-base/net/InetConnector.h>
#include <cortex-base/WallClock.h>
#include <cortex-base/RuntimeError.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpOutput.h>
#include <cortex-http/http1/Http1ConnectionFactory.h>
#include <unistd.h>

int main() {
  cortex::ThreadedExecutor threadedExecutor;
  cortex::Server server;
  cortex::WallClock* clock = cortex::WallClock::monotonic();
  bool shutdown = false;

  auto inet = server.addConnector<cortex::InetConnector>(
      "http", &threadedExecutor, nullptr, nullptr, clock,
      cortex::TimeSpan::fromSeconds(30),
      cortex::TimeSpan::Zero,
      &cortex::logAndPass,
      cortex::IPAddress("0.0.0.0"), 3000, 128, true, false);
  inet->setBlocking(true);

  auto http = inet->addConnectionFactory<cortex::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, cortex::TimeSpan::fromMinutes(3));

  http->setHandler([](cortex::HttpRequest* request, cortex::HttpResponse* response) {
    cortex::BufferRef body = "Hello, World\n";

    response->setStatus(cortex::HttpStatus::Ok);
    response->setContentLength(body.size());

    response->output()->write(body, std::bind(&cortex::HttpResponse::completed,
                                              response));
  });

  server.start();

  while (!shutdown)
    sleep(1);

  server.stop();

  return 0;
}
