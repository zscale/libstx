// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <cortex-http/Api.h>
#include <cortex-http/fastcgi/RequestParser.h>
#include <cortex-http/fastcgi/Generator.h>
#include <cortex-http/HttpTransport.h>
#include <cortex-http/HttpHandler.h>
#include <cortex-base/Buffer.h>
#include <cortex-base/TimeSpan.h>
#include <memory>
#include <unordered_map>

namespace cortex {

class HttpDateGenerator;
class HttpOutputCompressor;
class HttpChannel;
class HttpListener;
class EndPointWriter;

namespace http {
namespace fastcgi {

/**
 * FastCGI request stream.
 *
 * Represents a single request stream inside a (multiplexed) FastCGI connection.
 */
struct Stream : public CgiParamStreamReader {
  Stream(int rid, EndPointWriter* w, HttpChannel* channel);

  int requestId;
  HttpChannel* channel;
  Generator generator;
  CompletionHandler onComplete; // app-completion handler

  // input
  std::vector<std::pair<std::string, std::string>> environment;
  size_t requestContentActualLength;
  size_t requestContentExpectedLength; // CONTENT_LENGTH env value
  bool paramsComplete; //!< parameters fully received
  bool aborted; //!< request got aborted (downstream side)

 private:
  void onParam(const char *name, size_t nameLen,
               const char *value, size_t valueLen) override;
};

} // namespace fastcgi
} // namespace http
} // namespace cortex
