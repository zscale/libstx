// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-http/HttpService.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpInput.h>
#include <cortex-http/HttpOutput.h>

#include <cortex-base/net/IPAddress.h>
#include <cortex-base/io/Filter.h>
#include <cortex-base/executor/NativeScheduler.h>
#include <cortex-base/WallClock.h>

#include <cctype>

class Capslock : public cortex::Filter {
 public:
  void filter(const cortex::BufferRef& input,
              cortex::Buffer* output,
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
class MyHandler : public cortex::HttpService::Handler {
 public:
  bool handleRequest(cortex::HttpRequest* request, cortex::HttpResponse* response) override {
    if (request->path() == "/") {
      response->setStatus(cortex::HttpStatus::Found);
      response->addHeader("Location", "/welcome");
      response->completed();
      return true;
    }

    if (request->path() == "/error") {
      response->sendError(cortex::HttpStatus::BadRequest, "Custom Error");
      return true;
    }

    if (request->path() == "/chunked") {
      // not invoking setContentLength() causes the response to be chunked.
      response->setStatus(cortex::HttpStatus::Ok);
      response->addHeader("Content-Type", "text/plain");
      response->output()->write("Hello!\n");
      response->output()->write("World!\n");
      response->completed();
      return true;
    }

    if (request->path() == "/trailer") {
      response->setStatus(cortex::HttpStatus::Ok);

      response->addHeader("Content-Type", "text/plain");

      response->registerTrailer("Word-Count");
      response->registerTrailer("Mood");

      cortex::BufferRef body = "Hello, World!\n";
      response->setContentLength(body.size());
      response->output()->write(body);

      response->setTrailer("Word-Count", "2");
      response->setTrailer("Mood", "Happy");

      response->completed();
      return true;
    }

    if (request->path() == "/capslock-filter") {
      response->setStatus(cortex::HttpStatus::Ok);

      cortex::Buffer body;
      for (const auto& field: request->headers())
        body << field.name() << " = " << field.value() << "\n";

      response->output()->addFilter(std::make_shared<Capslock>());
      response->setContentLength(body.size());
      response->output()->write(std::move(body));
      response->completed();
      return true;
    }

    if (request->path() == "/headers") {
      cortex::Buffer body;
      for (const auto& field: request->headers()) {
        body << field.name() << " = " << field.value() << "\n";
      }
      response->setStatus(cortex::HttpStatus::Ok);
      response->addHeader("Content-Type", "text/plain");
      response->output()->write(body);
      response->completed();
      return true;
    }

    if (request->path() == "/echo") {
      cortex::Buffer body;
      request->input()->read(&body);
      response->setStatus(cortex::HttpStatus::Ok);
      response->setContentLength(body.size());
      response->output()->write(std::move(body));
      response->completed();
      return true;
    }

    if (request->path() == "/welcome") {
      response->setStatus(cortex::HttpStatus::Ok);
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
  cortex::NativeScheduler scheduler;
  cortex::WallClock* clock = cortex::WallClock::monotonic();

  MyHandler myHandler;
  cortex::HttpService::BuiltinAssetHandler builtinAssets;
  cortex::HttpService httpService;

  httpService.configureInet(&scheduler, &scheduler, clock,
                            cortex::TimeSpan::fromSeconds(10),
                            cortex::TimeSpan::Zero,
                            cortex::IPAddress("0.0.0.0"), 3000);
  httpService.addHandler(&myHandler);
  httpService.addHandler(&builtinAssets);

  builtinAssets.addAsset("/base.css", "text/css", "h2 { color: green; }\n");

  httpService.start();
  scheduler.runLoop();
  httpService.stop();

  return 0;
}
