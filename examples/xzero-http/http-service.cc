// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-http/HttpService.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpInput.h>
#include <xzero-http/HttpOutput.h>

#include <xzero-base/net/IPAddress.h>
#include <xzero-base/io/Filter.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/WallClock.h>

#include <cctype>

class Capslock : public xzero::Filter {
 public:
  void filter(const xzero::BufferRef& input,
              xzero::Buffer* output,
              bool last) override {
    printf("Capslock.filter(size=%zu, last=%s)\n",
        input.size(), last ? "true" : "false");
    output->reserve(input.size());
    for (char ch: input) {
      output->push_back(static_cast<char>(std::toupper(ch)));
    }
  }
};

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

    if (request->path() == "/error") {
      response->sendError(xzero::HttpStatus::BadRequest, "Custom Error");
      return true;
    }

    if (request->path() == "/chunked") {
      // not invoking setContentLength() causes the response to be chunked.
      response->setStatus(xzero::HttpStatus::Ok);
      response->addHeader("Content-Type", "text/plain");
      response->output()->write("Hello!\n");
      response->output()->write("World!\n");
      response->completed();
      return true;
    }

    if (request->path() == "/trailer") {
      response->setStatus(xzero::HttpStatus::Ok);

      response->addHeader("Content-Type", "text/plain");

      response->registerTrailer("Word-Count");
      response->registerTrailer("Mood");

      xzero::BufferRef body = "Hello, World!\n";
      response->setContentLength(body.size());
      response->output()->write(body);

      response->setTrailer("Word-Count", "2");
      response->setTrailer("Mood", "Happy");

      response->completed();
      return true;
    }

    if (request->path() == "/capslock-filter") {
      response->setStatus(xzero::HttpStatus::Ok);

      xzero::Buffer body;
      for (const auto& field: request->headers())
        body << field.name() << " = " << field.value() << "\n";

      response->output()->addFilter(std::make_shared<Capslock>());
      response->setContentLength(body.size());
      response->output()->write(std::move(body));
      response->completed();
      return true;
    }

    if (request->path() == "/headers") {
      xzero::Buffer body;
      for (const auto& field: request->headers()) {
        body << field.name() << " = " << field.value() << "\n";
      }
      response->setStatus(xzero::HttpStatus::Ok);
      response->addHeader("Content-Type", "text/plain");
      response->output()->write(body);
      response->completed();
      return true;
    }

    if (request->path() == "/echo") {
      xzero::Buffer body;
      request->input()->read(&body);
      response->setStatus(xzero::HttpStatus::Ok);
      response->setContentLength(body.size());
      response->output()->write(std::move(body));
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
  xzero::NativeScheduler scheduler;
  xzero::WallClock* clock = xzero::WallClock::monotonic();

  MyHandler myHandler;
  xzero::HttpService::BuiltinAssetHandler builtinAssets;
  xzero::HttpService httpService;

  httpService.configureInet(&scheduler, &scheduler, clock,
                            xzero::TimeSpan::fromSeconds(10),
                            xzero::TimeSpan::Zero,
                            xzero::IPAddress("0.0.0.0"), 3000);
  httpService.addHandler(&myHandler);
  httpService.addHandler(&builtinAssets);

  builtinAssets.addAsset("/base.css", "text/css", "h2 { color: green; }\n");

  httpService.start();
  scheduler.runLoop();
  httpService.stop();

  return 0;
}
