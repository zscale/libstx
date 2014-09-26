#pragma once

#include <xzero/Api.h>
#include <xzero/net/ConnectionFactory.h>
#include <xzero/http/HttpHandler.h>
#include <xzero/http/HttpDateGenerator.h>
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

  const HttpHandler& handler() const { return handler_; }
  void setHandler(HttpHandler&& handler);

  WallClock* clock() const { return clock_; }

  HttpDateGenerator* dateGenerator() const { return dateGenerator_.get(); }

  Connection* configure(Connection* connection, Connector* connector) override;

 private:
  size_t maxRequestUriLength_;
  size_t maxRequestBodyLength_;
  HttpHandler handler_;
  WallClock* clock_;
  std::unique_ptr<HttpDateGenerator> dateGenerator_;
};

}  // namespace xzero
