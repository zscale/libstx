// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-base/RuntimeError.h>
#include <cortex-base/WallClock.h>
#include <cortex-base/executor/NativeScheduler.h>
#include <cortex-base/executor/DirectExecutor.h>
#include <cortex-base/net/Server.h>
#include <cortex-base/net/InetConnector.h>
#include <cortex-base/net/SslConnector.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpOutput.h>
#include <cortex-http/http1/Http1ConnectionFactory.h>
#include <cortex-base/logging.h>

std::unique_ptr<cortex::SslConnector> createSslConnector( // {{{
    const std::string& name, int port, cortex::Executor* executor,
    cortex::Scheduler* scheduler, cortex::WallClock* clock) {

  std::unique_ptr<cortex::SslConnector> connector(
      new cortex::SslConnector(name, executor, scheduler, clock,
                              cortex::TimeSpan::fromSeconds(30),
                              cortex::TimeSpan::Zero,
                              [](const std::exception& e) {
                                  cortex::logError("hello", e); },
                              cortex::IPAddress("0.0.0.0"), port, 128,
                              true, true));

  connector->addContext("../../server.crt", "../../server.key");

  return connector;
}
// }}}

int main() {
  auto clock = cortex::WallClock::monotonic();
  cortex::NativeScheduler scheduler;
  cortex::Server server;

  auto inet = server.addConnector<cortex::InetConnector>(
      "http", &scheduler, &scheduler,  clock,
      cortex::TimeSpan::fromSeconds(60),
      cortex::TimeSpan::Zero,
      &cortex::logAndPass,
      cortex::IPAddress("0.0.0.0"), 3000, 128, true, false);
  inet->setBlocking(false);

  auto https = createSslConnector("https", 3443, &scheduler, &scheduler, clock);

  auto http = inet->addConnectionFactory<cortex::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, cortex::TimeSpan::fromMinutes(3));

  auto handler = [&](cortex::HttpRequest* request,
                     cortex::HttpResponse* response) {
    response->setStatus(cortex::HttpStatus::Ok);
    response->setReason("because");

    if (request->path() == "/bye") {
      server.stop();
    }

    cortex::Buffer body;
    body << "Hello " << request->path() << "\n";
    response->setContentLength(body.size());
    response->output()->write(
        std::move(body),
        std::bind(&cortex::HttpResponse::completed, response));
  };

  http->setHandler(handler);

  https->addConnectionFactory<cortex::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, cortex::TimeSpan::fromMinutes(3))->setHandler(handler);

  server.addConnector(std::move(https));

  server.start();
  scheduler.runLoop();
  server.stop();

  return 0;
}
