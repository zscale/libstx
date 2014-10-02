#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpVersion.h>
#include <xzero/http/HttpStatus.h>
#include <xzero/http/HttpOutput.h>
#include <xzero/http/HeaderFieldList.h>
#include <memory>

namespace xzero {

class HttpChannel;
class HttpOutput;

/**
 * Represents an HTTP response message.
 *
 * Semantic HTTP-protocol headers, such as "Date" will must not be added
 * as they are added by the HttpGenerator when flushing the response
 * to the client.
 *
 * @note It is not safe to mutate a response from multiple threads concurrently.
 */
class XZERO_API HttpResponse {
 private:
  HttpResponse(HttpResponse&) = delete;
  HttpResponse& operator=(HttpResponse&) = delete;

 public:
  HttpResponse(HttpChannel* channel, std::unique_ptr<HttpOutput>&& output);

  void recycle();

  HttpVersion version() const noexcept;
  void setVersion(HttpVersion version);

  void setStatus(HttpStatus status);
  HttpStatus status() const noexcept { return status_; }
  bool hasStatus() const noexcept { return status_ != HttpStatus::Undefined; }

  const std::string& reason() const noexcept { return reason_; }
  void setReason(const std::string& val);

  void setContentLength(size_t size);

  size_t contentLength() const noexcept {
    return contentLength_;
  }

  bool hasContentLength() const noexcept {
    return contentLength_ != static_cast<size_t>(-1);
  }

  void addHeader(const std::string& name, const std::string& value);
  void appendHeader(const std::string& name, const std::string& value,
                    const std::string& delim = "");
  void setHeader(const std::string& name, const std::string& value);
  void removeHeader(const std::string& name);
  void removeAllHeaders();
  const std::string& getHeader(const std::string& name) const;
  const HeaderFieldList& headers() const noexcept { return headers_; }
  HeaderFieldList& headers() noexcept { return headers_; }

  /**
   * Invoke to mark this response as complete.
   *
   * Further access to this object is undefined.
   */
  void completed();

  /**
   * Invoke to tell the client that it may continue sending the request body.
   *
   * You may only invoke this method if and only if the client actually
   * requested this behaviour via <code>Expect: 100-continue</code>
   * request header.
   */
  void send100Continue();

  /**
   * Responds with an error response message.
   *
   * @param code HTTP response status code.
   * @param message optional custom error message.
   *
   * @note This message is considered completed after this call.
   */
  void sendError(HttpStatus code, const std::string& message = "");

  HttpOutput* output() { return output_.get(); }

  bool isCommitted() const noexcept { return committed_; }
  void setCommitted(bool value);

 private:
  HttpChannel* channel_;
  std::unique_ptr<HttpOutput> output_;

  HttpVersion version_;
  HttpStatus status_;
  std::string reason_;
  size_t contentLength_;
  HeaderFieldList headers_;
  bool committed_;
};

}  // namespace xzero
