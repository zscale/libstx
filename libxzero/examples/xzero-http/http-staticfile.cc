// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/executor/DirectExecutor.h>
#include <xzero-base/executor/NativeScheduler.h>
#include <xzero-base/WallClock.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/net/Server.h>
#include <xzero-base/net/InetConnector.h>
#include <xzero-base/io/FileUtil.h>
#include <xzero-base/MimeTypes.h>
#include <xzero-base/cli/CLI.h>

#include <xzero-base/io/LocalFileRepository.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/HttpOutputCompressor.h>
#include <xzero-http/HttpFileHandler.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>

#include <iostream>

int main(int argc, const char* argv[]) {
  xzero::NativeScheduler scheduler;
  xzero::WallClock* clock = xzero::WallClock::monotonic();

  xzero::CLI cli;
  cli.defineBool("help", 'h', "Prints this help and terminates.")
     .defineIPAddress("bind", 0, "<IP>", "Bind listener to given IP.",
         xzero::IPAddress("0.0.0.0"))
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
  docroot = xzero::FileUtil::realpath(docroot);

  xzero::Server server;
  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      xzero::TimeSpan::fromSeconds(30),
      xzero::TimeSpan::Zero,
      &xzero::logAndPass,
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  xzero::HttpOutputCompressor* compressor = http->outputCompressor();
  compressor->setMinSize(5);

  xzero::MimeTypes mimetypes("/etc/mime.types", "application/octet-stream");
  xzero::LocalFileRepository localFiles(mimetypes, "/", true, true, true);
  xzero::HttpFileHandler fileHandler(localFiles);

  http->setHandler([&](xzero::HttpRequest* request, xzero::HttpResponse* response) {
    if (!fileHandler.handle(request, response, docroot)) {
      response->setStatus(xzero::HttpStatus::NotFound);
      response->completed();
    }
  });

  server.start();
  scheduler.runLoop();
  server.stop();
  return 0;
}
