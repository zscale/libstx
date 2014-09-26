#pragma once

#include <xzero/Api.h>
#include <xzero/TimeSpan.h>
#include <xzero/http/HttpConnectionFactory.h>

namespace xzero {
namespace http1 {

class XZERO_API Http1ConnectionFactory : public xzero::HttpConnectionFactory {
 public:
  Http1ConnectionFactory(
      size_t maxRequestUriLength,
      size_t maxRequestBodyLength,
      size_t maxRequestCount,
      TimeSpan maxKeepAlive);

  ~Http1ConnectionFactory();

  Connection* create(Connector* connector,
                     std::shared_ptr<EndPoint> endpoint) override;

  size_t maxRequestCount() const noexcept { return maxRequestCount_; }
  void setMaxRequestCount(size_t value) { maxRequestCount_ = value; }

  TimeSpan maxKeepAlive() const noexcept { return maxKeepAlive_; }
  void setMaxKeepAlive(TimeSpan value) { maxKeepAlive_ = value; }

 private:
  size_t maxRequestCount_;
  TimeSpan maxKeepAlive_;
};

}  // namespace http1
}  // namespace xzero
