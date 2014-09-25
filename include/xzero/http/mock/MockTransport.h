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
 *
 * This HTTP transport implementation is not using byte streams to communicate
 * but high level data structures to create requests and provides
 * access to high level data structures to read out the response message.
 *
 * The Executor service is only used for completion handlers.
 *
 * The actual HTTP request message handler is custom passed to the constructor.
 *
 * @note This object is not thread safe.
 *
 * @see HttpResponseInfo
 * @see HttpHandler
 * @see Executor
 */
class XZERO_API MockTransport : public HttpTransport {
 public:
  /**
   * Initializes the mock transport object.
   *
   * @param executor the executor service used for completion handlers.
   * @param handler the handler to run for incoming HTTP request messages.
   */
  explicit MockTransport(Executor* executor, const HttpHandler& handler);
  ~MockTransport();

  /**
   * Runs given HTTP request message.
   *
   * @param version HTTP message version, such as @c HttpVersion::VERSION_1_1.
   * @param method HTTP request method, such as @c "GET".
   * @param entity HTTP request entity, such as @c "/index.html".
   * @param headers HTTP request headers.
   * @param body HTTP request body.
   *
   * @see responseInfo()
   * @see responseBody()
   */
  void run(HttpVersion version, const std::string& method,
           const std::string& entity, const HeaderFieldList& headers,
           const std::string& body);

  /** Retrieves the response message status line and headers. */
  const HttpResponseInfo& responseInfo() const noexcept { return responseInfo_; }

  /** Retrieves the response message body. */
  const Buffer& responseBody() const noexcept { return responseBody_; }

  /** Tests whether last this transport was aborted in last request handling. */
  bool isAborted() const noexcept { return isAborted_; }

  /** Tests whether last message. */
  bool isCompleted() const noexcept { return isCompleted_; }

 private:
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

 private:
  Executor* executor_;
  HttpHandler handler_;

  bool isAborted_;
  bool isCompleted_;
  std::unique_ptr<HttpChannel> channel_;
  HttpResponseInfo responseInfo_;
  Buffer responseBody_;
};

} // namespace xzero
