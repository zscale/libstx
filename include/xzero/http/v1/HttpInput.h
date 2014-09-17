#pragma once

#include <xzero/http/HttpInput.h>
#include <xzero/Buffer.h>

namespace xzero {
namespace http1 {

class HttpConnection;

/**
 * HTTP/1 message body consumer.
 */
class HttpInput : public xzero::HttpInput {
 public:
  explicit HttpInput(HttpConnection* connection);
  ~HttpInput();

  int read(Buffer* result) override;
  size_t readLine(Buffer* result) override;
  void onContent(const BufferRef& chunk) override;

 private:
  HttpConnection* connection_;
  Buffer content_;
  size_t offset_;
};

}  // namespace http1
}  // namespace xzero
