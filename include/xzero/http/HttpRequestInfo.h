#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpInfo.h>
#include <string>

namespace xzero {

/**
 * HTTP Request Message Info.
 */
class XZERO_API HttpRequestInfo : public HttpInfo {
 public:
  HttpRequestInfo();
  HttpRequestInfo(HttpVersion version, const std::string& method,
                  const std::string& entity, size_t contentLength,
                  const HeaderFieldList& headers);

  const std::string& method() const noexcept { return method_; }
  const std::string& entity() const noexcept { return entity_; }

 private:
  std::string method_;
  std::string entity_;
};

inline HttpRequestInfo::HttpRequestInfo()
    : HttpRequestInfo(HttpVersion::UNKNOWN, "", "", 0, {}) {
}

inline HttpRequestInfo::HttpRequestInfo(HttpVersion version,
                                        const std::string& method,
                                        const std::string& entity,
                                        size_t contentLength,
                                        const HeaderFieldList& headers)
    : HttpInfo(version, contentLength, headers, {}),
      method_(method),
      entity_(entity) {
}

}  // namespace xzero

