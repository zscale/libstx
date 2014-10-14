#include <xzero/executor/ThreadedExecutor.h>
#include <xzero/net/Server.h>
#include <xzero/net/InetConnector.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/v1/Http1ConnectionFactory.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <xzero/logging/LogAggregator.h>
#include <xzero/logging/LogTarget.h>
#include <unistd.h>
#include <ev++.h>

int main() {
  xzero::LogAggregator::get().setLogLevel(xzero::LogLevel::Trace);
  xzero::LogAggregator::get().setLogTarget(xzero::LogTarget::console());

  xzero::ThreadedExecutor threadedExecutor;
  xzero::Server server;
  xzero::WallClock* clock = xzero::WallClock::system();
  bool shutdown = false;

  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &threadedExecutor, nullptr, nullptr, clock,
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  inet->setBlocking(true);

  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  http->setHandler([](xzero::HttpRequest* request, xzero::HttpResponse* response) {
    const xzero::Buffer body = "Hello, World\n";

    response->setStatus(xzero::HttpStatus::Ok);
    response->setContentLength(body.size());
    response->output()->write(body);
    response->completed();
  });

  server.start();

  while (!shutdown)
    sleep(1);

  server.stop();

  return 0;
}
