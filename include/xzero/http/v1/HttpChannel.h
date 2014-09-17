#pragma once

#include <xzero/http/HttpChannel.h>

namespace xzero {
namespace http1 {

class HttpChannel : public xzero::HttpChannel {
 public:
  HttpChannel(HttpTransport* transport, const HttpHandler& handler,
              std::unique_ptr<xzero::HttpInput>&& input);
  ~HttpChannel();

  bool isPersistent() const noexcept { return persistent_; }
  void setPersistent(bool value) noexcept { persistent_ = value; }

 protected:
  bool onMessageBegin(const BufferRef& method, const BufferRef& entity,
                      int versionMajor, int versionMinor) override;
  bool onMessageHeader(const BufferRef& name, const BufferRef& value) override;

 private:
  bool persistent_;
};


} // namespace http1
} // namespace xzero
