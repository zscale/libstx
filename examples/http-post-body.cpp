// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero/executor/DirectExecutor.h>
#include <xzero/executor/NativeScheduler.h>
#include <xzero/WallClock.h>
#include <xzero/net/Server.h>
#include <xzero/net/InetConnector.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/HttpInput.h>
#include <xzero/http/HttpInputListener.h>
#include <xzero/http/v1/Http1ConnectionFactory.h>
#include <xzero/logging/LogTarget.h>
#include <xzero/logging/LogAggregator.h>
#include <vector>
#include <string>

class HttpEcho : public xzero::HttpInputListener {
 public:
  HttpEcho(xzero::HttpRequest* req, xzero::HttpResponse* resp)
      : request_(req), response_(resp), bodyChunk_()
  {
    printf("going\n");
    if (request_->expect100Continue())
      response_->send100Continue(std::bind(&HttpEcho::go, this));
    else
      go();
  }

  void go() {
    printf("go\n");
    request_->input()->setListener(this);
    response_->setStatus(xzero::HttpStatus::Ok);

    for (const xzero::HeaderField& field : request_->headers()) {
      printf("[Header] %s: %s\n", field.name().c_str(), field.value().c_str());
    }
  }

  void onContentAvailable() override {
    printf("onContentAvailable\n");
    request_->input()->read(&bodyChunk_);
    response_->output()->write(bodyChunk_);
    bodyChunk_.clear();
  }

  void onAllDataRead() override {
    printf("onAllDataRead ;-)\n");
    response_->completed();
    delete this;
  }

  void onError(const std::string& errorMessage) override {
    printf("onError: %s\n", errorMessage.c_str());
    response_->completed();
    delete this;
  }

 private:
  xzero::HttpRequest* request_;
  xzero::HttpResponse* response_;
  xzero::Buffer bodyChunk_;
};

int main() {
  xzero::LogAggregator::get().setLogTarget(xzero::LogTarget::console());
  xzero::LogAggregator::get().setLogLevel(xzero::LogLevel::Trace);

  xzero::NativeScheduler scheduler;
  xzero::WallClock* clock = xzero::WallClock::monotonic();

  xzero::Server server;
  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      xzero::TimeSpan::fromSeconds(30),
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  http->setHandler([](xzero::HttpRequest* request,
                      xzero::HttpResponse* response) {
    new HttpEcho(request, response);
  });

  server.start();
  scheduler.runLoop();
  server.stop();
  return 0;
}
