#include <xzero/executor/DirectExecutor.h>
#include <xzero/net/Server.h>
#include <xzero/net/InetConnector.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/v1/HttpConnectionFactory.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <ev++.h>

int main() {
  ev::loop_ref loop = ev::default_loop(0);
  xzero::support::LibevScheduler scheduler(loop);
  xzero::support::LibevSelector selector(loop);
  xzero::support::LibevClock clock(loop);

  xzero::Server server;
  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &scheduler, &scheduler, &selector, xzero::IPAddress("0.0.0.0"),
      3000, 128, true, false);
  auto http = inet->addConnectionFactory<xzero::http1::HttpConnectionFactory>();
  http->setClock(&clock);

  http->setHandler([](xzero::HttpRequest* request,
                      xzero::HttpResponse* response) {
    response->setStatus(xzero::HttpStatus::Ok);
    response->setReason("because");

    response->output()->write("Hello");
    response->output()->write(" World!\n");
    response->completed();
  });

  server.start();
  selector.select();
  server.stop();
  return 0;
}
