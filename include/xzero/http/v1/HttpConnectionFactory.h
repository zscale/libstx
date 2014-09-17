#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpConnectionFactory.h>

namespace xzero {
namespace http1 {

class XZERO_API HttpConnectionFactory : public xzero::HttpConnectionFactory {
 public:
  HttpConnectionFactory();
  ~HttpConnectionFactory();

  Connection* create(Connector* connector,
                     std::shared_ptr<EndPoint> endpoint) override;
};

}  // namespace http1
}  // namespace xzero
