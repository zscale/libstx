#include <xzero/executor/DirectExecutor.h>
#include <xzero/net/Server.h>
#include <xzero/net/InetConnector.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/HttpFileHandler.h>
#include <xzero/http/v1/HttpConnectionFactory.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <xzero/io/FileRef.h>
#include <ev++.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, const char* argv[]) {
  ev::loop_ref loop = ev::default_loop(0);
  xzero::support::LibevScheduler scheduler(loop);
  xzero::support::LibevSelector selector(loop);
  xzero::support::LibevClock clock(loop);

  std::string docroot = argc == 2 ? argv[1] : ".";
  xzero::Server server;
  auto inet = server.addConnector<xzero::InetConnector>("http", &scheduler,
      &scheduler, &selector, xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  auto http = inet->addConnectionFactory<xzero::http1::HttpConnectionFactory>();
  http->setClock(&clock);

  http->setHandler([&](xzero::HttpRequest* request, xzero::HttpResponse* response) {
    xzero::HttpFileHandler fileHandler(true, true, true);
    fileHandler.handle(request, response, docroot);
  });

  server.start();
  selector.select();
  server.stop();
  return 0;
}
