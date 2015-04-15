// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

/*
 * XXX demo: MTB request handler with STNB I/O.
 */

#include <cortex-base/executor/ThreadPool.h>
#include <cortex-base/executor/NativeScheduler.h>
#include <cortex-base/net/Server.h>
#include <cortex-base/net/InetConnector.h>
#include <cortex-base/WallClock.h>
#include <cortex-base/TimeSpan.h>
#include <cortex-base/RuntimeError.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpOutput.h>
#include <cortex-http/http1/Http1ConnectionFactory.h>
#include <unistd.h>

void runJob(cortex::HttpRequest* request, cortex::HttpResponse* response, cortex::Executor* context) {
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = cortex::TimeSpan(0.1).nanoseconds();
  for (;;) {
    printf("request. job: nanosleep()ing %li\n", ts.tv_nsec);
    if (nanosleep(&ts, &ts) == 0 || errno != EINTR)
      break;
  }

  // run the complex stuff here
  cortex::BufferRef body = "Hello, World\n";

  // now respond to the client
  context->execute([=]() {
    printf("request. response\n");
    response->setStatus(cortex::HttpStatus::Ok);
    response->setContentLength(body.size());

    response->output()->write(body, std::bind(&cortex::HttpResponse::completed,
                                              response));
  });
}

int main() {
  cortex::NativeScheduler scheduler;
  cortex::WallClock* clock = cortex::WallClock::monotonic();

  //cortex::ThreadPool threaded(16);
  cortex::Server server;
  bool shutdown = false;

  auto inet = server.addConnector<cortex::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      cortex::TimeSpan::fromSeconds(30),
      cortex::TimeSpan::fromSeconds(30),
      cortex::TimeSpan::Zero,
      &cortex::logAndPass,
      cortex::IPAddress("0.0.0.0"), 3000, 128, true, false);

  auto http = inet->addConnectionFactory<cortex::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, cortex::TimeSpan::fromMinutes(3));

  http->setHandler([&](cortex::HttpRequest* request, cortex::HttpResponse* response) {
    printf("request\n");
    scheduler.execute(std::bind(&runJob, request, response, &scheduler));
    //threaded.execute(std::bind(&runJob, request, response, &scheduler));
  });

  server.start();
  scheduler.runLoop();
  server.stop();
  return 0;
}
