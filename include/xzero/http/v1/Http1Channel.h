#pragma once

#include <xzero/http/HttpChannel.h>
#include <list>
#include <string>

namespace xzero {
namespace http1 {

class Http1Channel : public xzero::HttpChannel {
 public:
  Http1Channel(HttpTransport* transport,
              const HttpHandler& handler,
              std::unique_ptr<HttpInput>&& input,
              size_t maxRequestUriLength,
              size_t maxRequestBodyLength,
              HttpOutputCompressor* outputCompressor);
  ~Http1Channel();

  bool isPersistent() const noexcept { return persistent_; }
  void setPersistent(bool value) noexcept { persistent_ = value; }

  virtual void reset();

 protected:
  bool onMessageBegin(const BufferRef& method, const BufferRef& entity,
                      HttpVersion version) override;
  bool onMessageHeader(const BufferRef& name, const BufferRef& value) override;
  bool onMessageHeaderEnd() override;
  void onProtocolError(HttpStatus code, const std::string& message) override;

 private:
  bool persistent_;
  std::list<std::string> connectionOptions_;
};

} // namespace http1
} // namespace xzero
