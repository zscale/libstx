#pragma once

#include <xzero/http/HttpInput.h>
#include <xzero/Buffer.h>

namespace xzero {
namespace http1 {

class HttpConnection;

/**
 * HTTP/1 message body consumer.
 */
class Http1Input : public xzero::HttpInput {
 public:
  explicit Http1Input(HttpConnection* connection);
  ~Http1Input();

  int read(Buffer* result) override;
  size_t readLine(Buffer* result) override;
  void onContent(const BufferRef& chunk) override;

  void recycle() override;

 private:
  HttpConnection* connection_;
  Buffer content_;
  size_t offset_;
};

}  // namespace http1
}  // namespace xzero
