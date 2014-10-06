#pragma once

#include <xzero/Api.h>
#include <xzero/net/ConnectionFactory.h>
#include <xzero/http/HttpHandler.h>
#include <xzero/http/HttpDateGenerator.h>
#include <xzero/http/HttpOutputCompressor.h>
#include <memory>

namespace xzero {

class WallClock;

/**
 * Base HTTP connection factory.
 *
 * This provides common functionality to all HTTP connection factories.
 */
class XZERO_API HttpConnectionFactory : public ConnectionFactory {
 public:
  HttpConnectionFactory(
      const std::string& protocolName,
      WallClock* clock,
      size_t maxRequestUriLength,
      size_t maxRequestBodyLength);

  ~HttpConnectionFactory();

  size_t maxRequestUriLength() const noexcept { return maxRequestUriLength_; }
  void setMaxRequestUriLength(size_t value) { maxRequestUriLength_ = value; }

  size_t maxRequestBodyLength() const noexcept { return maxRequestBodyLength_; }
  void setMaxRequestBodyLength(size_t value) { maxRequestBodyLength_ = value; }

  const HttpHandler& handler() const noexcept { return handler_; }
  void setHandler(HttpHandler&& handler);

  WallClock* clock() const noexcept;

  HttpOutputCompressor* outputCompressor() const noexcept;

  HttpDateGenerator* dateGenerator() const noexcept;

  Connection* configure(Connection* connection, Connector* connector) override;

 private:
  size_t maxRequestUriLength_;
  size_t maxRequestBodyLength_;
  HttpHandler handler_;
  WallClock* clock_;
  std::unique_ptr<HttpOutputCompressor> outputCompressor_;
  std::unique_ptr<HttpDateGenerator> dateGenerator_;
};

// {{{ inlines
inline WallClock* HttpConnectionFactory::clock() const noexcept {
  return clock_;
}

inline HttpOutputCompressor* HttpConnectionFactory::outputCompressor() const noexcept {
  return outputCompressor_.get();
}

inline HttpDateGenerator* HttpConnectionFactory::dateGenerator() const noexcept {
  return dateGenerator_.get();
}
// }}}

}  // namespace xzero
