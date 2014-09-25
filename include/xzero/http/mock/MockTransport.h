#pragma once

#include <xzero/http/HttpTransport.h>
#include <xzero/http/HttpHandler.h>
#include <xzero/http/HttpResponseInfo.h>
#include <xzero/http/HttpChannel.h>
#include <xzero/Buffer.h>
#include <xzero/Api.h>

namespace xzero {

class Executor;

/**
 * Mock HTTP Transport, used to create mock requests.
 */
class XZERO_API MockTransport : public HttpTransport {
 public:
  explicit MockTransport(Executor* executor, const HttpHandler& handler);
  ~MockTransport();

  void run(HttpVersion version, const std::string& method,
           const std::string& entity, const HeaderFieldList& headers,
           const std::string& body);

  // HttpTransport overrides
  void abort() override;
  void completed() override;
  void send(HttpResponseInfo&& responseInfo, const BufferRef& chunk,
            CompletionHandler&& onComplete) override;
  void send(const BufferRef& chunk, CompletionHandler&& onComplete) override;
  void send(FileRef&& chunk, CompletionHandler&& onComplete) override;

  // Connection overrides
  void onOpen() override;
  void onClose() override;
  void onFillable() override;
  void onFlushable() override;
  bool onReadTimeout() override;

  const HttpResponseInfo& responseInfo() const noexcept { return responseInfo_; }
  const Buffer& responseBody() const noexcept { return responseBody_; }

 private:
  Executor* executor_;
  HttpHandler handler_;

  std::unique_ptr<HttpChannel> channel_;
  HttpResponseInfo responseInfo_;
  Buffer responseBody_;
};

} // namespace xzero
