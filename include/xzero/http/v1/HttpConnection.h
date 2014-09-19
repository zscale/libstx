#pragma once

#include <xzero/Api.h>
#include <xzero/Buffer.h>
#include <xzero/TimeSpan.h>
#include <xzero/net/EndPointWriter.h>
#include <xzero/http/HttpTransport.h>
#include <xzero/http/HttpHandler.h>
#include <xzero/http/v1/HttpParser.h>
#include <xzero/http/v1/HttpGenerator.h>
#include <memory>

namespace xzero {

class HttpDateGenerator;

namespace http1 {

class HttpInput;
class HttpChannel;

/**
 * @brief Implements a HTTP/1.1 transport connection.
 */
class XZERO_API HttpConnection : public HttpTransport {
 public:
  HttpConnection(std::shared_ptr<EndPoint> endpoint,
                 const HttpHandler& handler, HttpDateGenerator* dateGenerator);
  ~HttpConnection();

  void onOpen() override;
  void onClose() override;

  void abort() override;
  void completed() override;

  void send(HttpResponseInfo&& responseInfo, const BufferRef& chunk,
            CompletionHandler&& onComplete) override;

  void send(const BufferRef& chunk, CompletionHandler&& onComplete) override;
  void send(FileRef&& chunk, CompletionHandler&& onComplete) override;

  void setInputBufferSize(size_t size) override;

 private:
  void onFillable() override;
  void onFlushable() override;
  void onInterestFailure(const std::exception& error) override;

 private:
  HttpParser parser_;
  HttpGenerator generator_;

  Buffer inputBuffer_;
  size_t inputOffset_;

  EndPointWriter writer_;
  CompletionHandler onComplete_;

  std::unique_ptr<HttpChannel> channel_;
  TimeSpan maxKeepAlive_;
  size_t requestCount_;
  size_t requestMax_;
};

}  // namespace http1
}  // namespace xzero
