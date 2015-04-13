// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cortex-base/executor/DirectExecutor.h>
#include <cortex-base/executor/NativeScheduler.h>
#include <cortex-base/WallClock.h>
#include <cortex-base/RuntimeError.h>
#include <cortex-base/net/Server.h>
#include <cortex-base/net/InetConnector.h>
#include <cortex-base/io/FileUtil.h>
#include <cortex-base/MimeTypes.h>
#include <cortex-base/cli/CLI.h>

#include <cortex-base/io/LocalFileRepository.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpOutput.h>
#include <cortex-http/HttpOutputCompressor.h>
#include <cortex-http/HttpFileHandler.h>
#include <cortex-http/http1/Http1ConnectionFactory.h>

#include <iostream>

int main(int argc, const char* argv[]) {
  cortex::NativeScheduler scheduler;
  cortex::WallClock* clock = cortex::WallClock::monotonic();

  cortex::CLI cli;
  cli.defineBool("help", 'h', "Prints this help and terminates.")
     .defineIPAddress("bind", 0, "<IP>", "Bind listener to given IP.",
         cortex::IPAddress("0.0.0.0"))
     .defineString("mimetypes", 0, "<PATH>", "Path to mime.types file.",
         "/etc/mime.types", nullptr)
     .defineNumber("port", 'p', "<PORT>", "Port number to listen to.",
         3000)
     .defineNumber("backlog", 0, "<COUNT>", "Listener backlog.",
         128)
     .defineNumber("timeout", 't', "<SECONDS>", "I/O timeout in seconds.",
         30)
     .defineString("very-long", 'L', "<TEXT>",
         "A very very long help text for the rather short long option. "
         "You can use it, but it will not produce or cause you anything "
         "but a word-wrapped help text in the help output. "
         "Have fun reading.",
         "rather long", nullptr)
     .defineBool("tcp-nodelay", 0, "Enables TCP_NODELAY.")
     .defineBool("tcp-quickack", 0, "Enables TCP_QUICKACK.")
     .enableParameters("<garbage>", "Defines a list of unparsed arguments.")
     ;

  std::cerr << "Parameters:" << std::endl
            << cli.helpText() << std::endl;

  return 0;

  std::string docroot = argc == 2 ? argv[1] : ".";
  docroot = cortex::FileUtil::realpath(docroot);

  cortex::Server server;
  auto inet = server.addConnector<cortex::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      cortex::TimeSpan::fromSeconds(30),
      cortex::TimeSpan::Zero,
      &cortex::logAndPass,
      cortex::IPAddress("0.0.0.0"), 3000, 128, true, false);
  auto http = inet->addConnectionFactory<cortex::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, cortex::TimeSpan::fromMinutes(3));

  cortex::HttpOutputCompressor* compressor = http->outputCompressor();
  compressor->setMinSize(5);

  cortex::MimeTypes mimetypes("/etc/mime.types", "application/octet-stream");
  cortex::LocalFileRepository vfs(mimetypes, "/", true, true, true);
  cortex::HttpFileHandler fileHandler;

  http->setHandler([&](cortex::HttpRequest* request, cortex::HttpResponse* response) {
    request->setFile(vfs.getFile(request->path(), docroot));
    if (!fileHandler.handle(request, response)) {
      response->setStatus(cortex::HttpStatus::NotFound);
      response->completed();
    }
  });

  server.start();
  scheduler.runLoop();
  server.stop();
  return 0;
}
