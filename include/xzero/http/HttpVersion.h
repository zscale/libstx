#pragma once

#include <string>
#include <xzero/Api.h>

namespace xzero {

/**
 * HTTP protocol version number.
 */
enum class HttpVersion {
  UNKNOWN = 0,
  VERSION_0_9 = 0x09,
  VERSION_1_0 = 0x10,
  VERSION_1_1 = 0x11,
  VERSION_2_0 = 0x20,
};

XZERO_API const std::string& to_string(HttpVersion value);

} // namespace xzero
