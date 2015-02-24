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
#include <xzero-base/logging/LogTarget.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/io/FileUtil.h>
#include <xzero-base/MimeTypes.h>

#include <xzero-http/HttpLocalFileRepository.h>
#include <xzero-http/HttpRequest.h>
#include <xzero-http/HttpResponse.h>
#include <xzero-http/HttpOutput.h>
#include <xzero-http/HttpOutputCompressor.h>
#include <xzero-http/HttpFileHandler.h>
#include <xzero-http/http1/Http1ConnectionFactory.h>

int main(int argc, const char* argv[]) {
  xzero::LogAggregator::get().setLogTarget(xzero::LogTarget::console());
  xzero::LogAggregator::get().setLogLevel(xzero::LogLevel::Trace);

  xzero::NativeScheduler scheduler;
  xzero::WallClock* clock = xzero::WallClock::monotonic();

  std::string docroot = argc == 2 ? argv[1] : ".";
  docroot = xzero::FileUtil::realpath(docroot);

  xzero::Server server;
  auto inet = server.addConnector<xzero::InetConnector>(
      "http", &scheduler, &scheduler, clock,
      xzero::TimeSpan::fromSeconds(30),
      &xzero::consoleLogger,
      xzero::IPAddress("0.0.0.0"), 3000, 128, true, false);
  auto http = inet->addConnectionFactory<xzero::http1::Http1ConnectionFactory>(
      clock, 100, 512, 5, xzero::TimeSpan::fromMinutes(3));

  xzero::HttpOutputCompressor* compressor = http->outputCompressor();
  compressor->setMinSize(5);

  xzero::MimeTypes mimetypes("/etc/mime.types", "application/octet-stream");
  xzero::HttpLocalFileRepository localFiles(mimetypes, "/", true, true, true);
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
