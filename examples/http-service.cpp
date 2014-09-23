#include <xzero/http/HttpService.h>
#include <xzero/http/HttpRequest.h>
#include <xzero/http/HttpResponse.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/net/IPAddress.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <ev++.h>

/**
 * Some custom HTTP request demo-handler.
 */
class MyHandler : public xzero::HttpService::Handler {
 public:
  bool handleRequest(xzero::HttpRequest* request, xzero::HttpResponse* response) override {
    if (request->path() == "/") {
      response->setStatus(xzero::HttpStatus::Found);
      response->addHeader("Location", "/welcome");
      response->completed();
      return true;
    }

    if (request->path() == "/welcome") {
      response->setStatus(xzero::HttpStatus::Ok);
      response->addHeader("Content-Type", "text/html");
      response->output()->write("<html>\n"
                                " <head>\n"
                                "   <link href='/base.css' rel='stylesheet'/>\n"
                                " </head>\n"
                                " <body> <h2>Hello, World </h2> </body>\n"
                                "</html>\n");
      response->completed();
      return true;
    }
    return false;
  }
};

int main(int argc, const char* argv[]) {
  ev::loop_ref loop = ev::default_loop(0);
  xzero::support::LibevScheduler scheduler(loop);
  xzero::support::LibevSelector selector(loop);

  MyHandler myHandler;
  xzero::HttpService::BuiltinAssetHandler builtinAssets;
  xzero::HttpService httpService;

  httpService.configureInet(&scheduler, &scheduler, &selector,
                            xzero::IPAddress("0.0.0.0"), 3000);
  httpService.addHandler(&myHandler);
  httpService.addHandler(&builtinAssets);

  builtinAssets.addAsset("/base.css", "text/css", "h2 { color: green; }\n");

  httpService.start();
  selector.select();
  httpService.stop();

  return 0;
}
