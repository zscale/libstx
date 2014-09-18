#include <xzero/executor/DirectExecutor.h>
#include <xzero/net/Server.h>
#include <xzero/net/InetConnector.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/HttpInput.h>
#include <xzero/http/HttpInputListener.h>
#include <xzero/http/v1/HttpConnectionFactory.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <vector>
#include <string>
#include <ev++.h>

class HttpEcho : public xzero::HttpInputListener {
 public:
  HttpEcho(xzero::HttpRequest* req, xzero::HttpResponse* resp)
      : request_(req), response_(resp), bodyChunk_() {

    request_->setHandled(true);

    // TODO: handle "Expect: 100-continue" nicely
    // if (request_->expects100Continue())
    //   response_->send100Continue();

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
    new HttpEcho(request, response);
  });

  server.start();
  selector.select();
  server.stop();
  return 0;
}
