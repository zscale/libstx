// This file is part of the "x0" project, http://cortex.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <cortex-http/Api.h>
#include <cortex-base/Buffer.h>
#include <cortex-base/TimeSpan.h>
#include <cortex-base/net/EndPointWriter.h>
#include <cortex-http/HttpTransport.h>
#include <cortex-http/HttpHandler.h>
#include <cortex-http/http1/HttpParser.h>
#include <cortex-http/http1/HttpGenerator.h>
#include <memory>

namespace cortex {

class HttpDateGenerator;
class HttpOutputCompressor;

namespace http1 {

class Http1Channel;

/**
 * @brief Implements a HTTP/1.1 transport connection.
 */
class XZERO_HTTP_API HttpConnection : public HttpTransport {
 public:
  HttpConnection(EndPoint* endpoint,
                 Executor* executor,
                 const HttpHandler& handler,
                 HttpDateGenerator* dateGenerator,
                 HttpOutputCompressor* outputCompressor,
                 size_t maxRequestUriLength,
                 size_t maxRequestBodyLength,
                 size_t maxRequestCount,
                 TimeSpan maxKeepAlive);
  ~HttpConnection();

  void onOpen() override;
  void onClose() override;

  void abort() override;
  void completed() override;

  void send(HttpResponseInfo&& responseInfo, Buffer&& chunk,
            CompletionHandler onComplete) override;
  void send(HttpResponseInfo&& responseInfo, const BufferRef& chunk,
            CompletionHandler onComplete) override;
  void send(HttpResponseInfo&& responseInfo, FileRef&& chunk,
            CompletionHandler onComplete) override;

  void send(Buffer&& chunk, CompletionHandler onComplete) override;
  void send(const BufferRef& chunk, CompletionHandler onComplete) override;
  void send(FileRef&& chunk, CompletionHandler onComplete) override;

  void setInputBufferSize(size_t size) override;

 private:
  void patchResponseInfo(HttpResponseInfo& info);
  void onFillable() override;
  void parseFragment();
  void onFlushable() override;
  void onInterestFailure(const std::exception& error) override;
  void onResponseComplete(bool succeed);

 private:
  HttpParser parser_;

  Buffer inputBuffer_;
  size_t inputOffset_;

  EndPointWriter writer_;
  CompletionHandler onComplete_;
  HttpGenerator generator_;

  std::unique_ptr<Http1Channel> channel_;
  TimeSpan maxKeepAlive_;
  size_t requestCount_;
  size_t requestMax_;
};

}  // namespace http1
}  // namespace cortex
