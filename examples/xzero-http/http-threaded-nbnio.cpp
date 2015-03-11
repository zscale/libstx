// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

/*
 * XXX demo: MTB request handler with STNB I/O.
 */

#include <xzero-base/executor/ThreadPool.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/TimeSpan.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>
#include <unistd.h>

void runJob(xzero::HttpRequest* request, xzero::HttpResponse* response, xzero::Executor* context) {
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = xzero::TimeSpan(0.1).nanoseconds();
  for (;;) {
    printf("request. job: nanosleep()ing %li\n", ts.tv_nsec);
    if (nanosleep(&ts, &ts) == 0 || errno != EINTR)
      break;
  }

  // run the complex stuff here
  xzero::BufferRef body = "Hello, World\n";

  // now respond to the client
  context->execute([=]() {
    printf("request. response\n");
    response->setStatus(xzero::HttpStatus::Ok);
    response->setContentLength(body.size());

    response->output()->write(body, std::bind(&xzero::HttpResponse::completed,
                                              response));
  });
}

int main() {
  xzero::NativeScheduler scheduler;
  xzero::WallClock* clock = xzero::WallClock::monotonic();

  //xzero::ThreadPool threaded(16);
  xzero::Server server;
  bool shutdown = false;

  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      xzero::TimeSpan::fromSeconds(30),
      xzero::TimeSpan::Zero,
      &xzero::logAndPass,
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);

  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  http->setHandler([&](xzero::HttpRequest* request, xzero::HttpResponse* response) {
    printf("request\n");
    scheduler.execute(std::bind(&runJob, request, response, &scheduler));
    //threaded.execute(std::bind(&runJob, request, response, &scheduler));
  });

  server.start();
  scheduler.runLoop();
  server.stop();
  return 0;
}
