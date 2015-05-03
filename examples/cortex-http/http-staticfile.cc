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
#include <cortex-base/cli/Flags.h>

#include <cortex-base/io/LocalFileRepository.h>
#include <cortex-http/HttpRequest.h>
#include <cortex-http/HttpResponse.h>
#include <cortex-http/HttpOutput.h>
#include <cortex-http/HttpOutputCompressor.h>
#include <cortex-http/HttpFileHandler.h>
#include <cortex-http/http1/Http1ConnectionFactory.h>
#include <cortex-http/fastcgi/ConnectionFactory.h>

#include <iostream>

using namespace cortex;

int main(int argc, const char* argv[]) {
  cortex::NativeScheduler scheduler;
  cortex::WallClock* clock = cortex::WallClock::monotonic();

  cortex::CLI cli;
  cli.defineBool("help", 'h', "Prints this help and terminates.")
     .defineIPAddress("bind", 0, "<IP>", "Bind listener to given IP.", cortex::IPAddress("0.0.0.0"))
     .defineNumber("port", 'p', "<PORT>", "Port number to listen to.", 3000)
     .defineNumber("backlog", 0, "<COUNT>", "Listener backlog.", 128)
     .defineNumber("timeout", 't', "<SECONDS>", "I/O timeout in seconds.", 30)
     .defineString("mimetypes", 0, "<PATH>", "Path to mime.types file.", "/etc/mime.types", nullptr)
     .defineString("docroot", 'R', "<PATH>", "Path to document root.", FileUtil::realpath("."), nullptr)
     .defineBool("tcp-nodelay", 0, "Enables TCP_NODELAY.")
     .defineBool("tcp-quickack", 0, "Enables TCP_QUICKACK.")
     .defineBool("fastcgi", 0, "Use FastCGI instead of HTTP/1 as transport protocol.")
     ;

  Flags flags = cli.evaluate(argc, argv);

  if (flags.getBool("help")) {
    std::cerr << "Parameters:" << std::endl
              << cli.helpText() << std::endl;
    return 0;
  }

  std::string docroot = cortex::FileUtil::realpath(flags.getString("docroot"));

  cortex::Server server;
  auto inet = server.addConnector<cortex::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      cortex::TimeSpan::fromSeconds(flags.getNumber("timeout")),
      cortex::TimeSpan::fromSeconds(flags.getNumber("timeout")),
      cortex::TimeSpan::Zero,
      &cortex::logAndPass,
      flags.getIPAddress("bind"),
      flags.getNumber("port"),
      flags.getNumber("backlog"),
      true,
      false);

  std::shared_ptr<HttpConnectionFactory> http;
  if (flags.getBool("fastcgi"))
    http = inet->addConnectionFactory<cortex::http::fastcgi::ConnectionFactory>(
        clock, 100, 512, cortex::TimeSpan::fromSeconds(8));
  else
    http = inet->addConnectionFactory<cortex::http1::Http1ConnectionFactory>(
        clock, 100, 512, 5, cortex::TimeSpan::fromSeconds(8));

  cortex::HttpOutputCompressor* compressor = http->outputCompressor();
  compressor->setMinSize(5);

  cortex::MimeTypes mimetypes(flags.getString("mimetypes"), "application/octet-stream");
  cortex::LocalFileRepository vfs(mimetypes, "/", true, true, true);
  cortex::HttpFileHandler fileHandler;

  http->setHandler([&](cortex::HttpRequest* request, cortex::HttpResponse* response) {
    auto file = vfs.getFile(request->path(), docroot);
    if (!fileHandler.handle(request, response, file)) {
      response->setStatus(cortex::HttpStatus::NotFound);
      response->completed();
    }
  });

  server.start();
  scheduler.runLoop();
  server.stop();
  return 0;
}
