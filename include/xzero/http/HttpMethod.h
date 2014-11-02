#pragma once

#include <xzero/Api.h>
#include <string>

namespace xzero {

enum class HttpMethod {
  UNKNOWN_METHOD,
  OPTIONS,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT,
};

XZERO_API std::string to_string(HttpMethod value);
XZERO_API HttpMethod to_method(const std::string& value);

} // namespace xzero
