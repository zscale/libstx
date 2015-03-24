// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/logging/LogTarget.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/HttpInput.h>
#include <xzero-http/HttpInputListener.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>
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
      xzero::TimeSpan::Zero,
      &xzero::logAndPass,
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
