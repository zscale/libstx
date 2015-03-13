// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/RuntimeError.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/net/SslConnector.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>
#include <xzero-base/logging.h>

std::unique_ptr<xzero::SslConnector> createSslConnector( // {{{
    const std::string& name, int port, xzero::Executor* executor,
    xzero::Scheduler* scheduler, xzero::WallClock* clock) {

  std::unique_ptr<xzero::SslConnector> connector(
      new xzero::SslConnector(name, executor, scheduler, clock,
                              xzero::TimeSpan::fromSeconds(30),
                              xzero::TimeSpan::Zero,
                              [](const std::exception& e) {
                                  xzero::logError("hello", e); },
                              xzero::IPAddress("0.0.0.0"), port, 128,
                              true, true));

  connector->addContext("../../server.crt", "../../server.key");

  return connector;
}
// }}}

int main() {
  auto clock = xzero::WallClock::monotonic();
  xzero::NativeScheduler scheduler;
  xzero::Server server;

  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &scheduler, &scheduler,  clock,
      xzero::TimeSpan::fromSeconds(60),
      xzero::TimeSpan::Zero,
      &xzero::logAndPass,
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  inet->setBlocking(false);

  auto https = createSslConnector("https", 3443, &scheduler, &scheduler, clock);

  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  auto handler = [&](xzero::HttpRequest* request,
                     xzero::HttpResponse* response) {
    response->setStatus(xzero::HttpStatus::Ok);
    response->setReason("because");

    if (request->path() == "/bye") {
      server.stop();
    }

    xzero::Buffer body;
    body << "Hello " << request->path() << "\n";
    response->setContentLength(body.size());
    response->output()->write(
        std::move(body),
        std::bind(&xzero::HttpResponse::completed, response));
  };

  http->setHandler(handler);

  https->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3))->setHandler(handler);

  server.addConnector(std::move(https));

  server.start();
  scheduler.runLoop();
  server.stop();

  return 0;
}
