// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogTarget.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/RuntimeError.h>

#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>

enum class MaybeRaise {
  Never,
  Now,
};

class MaybeRaiseCategory : public std::error_category {
 public:
  static std::error_category& get() {
    static MaybeRaiseCategory ec;
    return ec;
  }
  const char* name() const noexcept override {
    return "maybe_raise";
  }
  std::string message(int ev) const override {
    switch (static_cast<MaybeRaise>(ev)) {
      case MaybeRaise::Now: return "Now";
      case MaybeRaise::Never: return "Never";
      default: return "Unknown MaybeRaise Error.";
    }
  }
};

using namespace xzero;

int main() {
  auto errorHandler = [](const std::exception& e) {
    xzero::logAndPass(e);
  };

  xzero::WallClock* clock = xzero::WallClock::monotonic();
  xzero::NativeScheduler scheduler(errorHandler, clock);

  scheduler.setExceptionHandler(errorHandler);

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
    if (request->path() == "/raise") {
      printf("blah\n");
      RAISE_CATEGORY(MaybeRaise::Now, MaybeRaiseCategory::get());
    }

    response->setStatus(xzero::HttpStatus::Ok);
    response->output()->write("Call me maybe /raise ;-)\n",
        std::bind(&xzero::HttpResponse::completed, response));
  });

  try {
    server.start();
    scheduler.runLoop();
    server.stop();
  } catch (...) {
  }
  return 0;
}
