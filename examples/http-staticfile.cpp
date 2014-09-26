#include <xzero/executor/DirectExecutor.h>
#include <xzero/net/Server.h>
#include <xzero/net/InetConnector.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/HttpFileHandler.h>
#include <xzero/http/v1/Http1ConnectionFactory.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <xzero/logging/LogTarget.h>
#include <xzero/logging/LogAggregator.h>
#include <ev++.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, const char* argv[]) {
  xzero::LogAggregator::get().setLogLevel(xzero::LogLevel::Trace);
  xzero::LogAggregator::get().setLogTarget(xzero::LogTarget::console());

  ev::loop_ref loop = ev::default_loop(0);
  xzero::support::LibevScheduler scheduler(loop);
  xzero::support::LibevSelector selector(loop);
  xzero::support::LibevClock clock(loop);

  std::string docroot = argc == 2 ? argv[1] : ".";
  xzero::Server server;
  auto inet = server.addConnector<xzero::InetConnector>("http", &scheduler,
      &scheduler, &selector, xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      100, 512, 5, xzero::TimeSpan::fromMinutes(3));
  http->setClock(&clock);

  http->setHandler([&](xzero::HttpRequest* request, xzero::HttpResponse* response) {
    xzero::HttpFileHandler fileHandler(true, true, true);
    if (!fileHandler.handle(request, response, docroot)) {
      response->setStatus(xzero::HttpStatus::NotFound);
      response->completed();
    }
  });

  server.start();
  selector.select();
  server.stop();
  return 0;
}
