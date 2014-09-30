#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpVersion.h>
#include <xzero/http/HeaderFieldList.h>
#include <string>

namespace xzero {

/**
 * Base HTTP Message Info.
 *
 * @see HttpRequestInfo
 * @see HttpResponseInfo
 */
class XZERO_API HttpInfo {
 public:
  HttpInfo(HttpVersion version, size_t contentLength,
           const HeaderFieldList& headers);

  /** Retrieves the HTTP message version. */
  HttpVersion version() const noexcept { return version_; }

  /** Retrieves the HTTP response headers. */
  const HeaderFieldList& headers() const noexcept { return headers_; }

  /** Retrieves the HTTP response headers. */
  HeaderFieldList& headers() noexcept { return headers_; }

  void setContentLength(size_t size);
  size_t contentLength() const noexcept { return contentLength_; }

  bool hasContentLength() const noexcept {
    return contentLength_ != static_cast<size_t>(-1);
  }

 protected:
  HttpVersion version_;
  HeaderFieldList headers_;
  size_t contentLength_;
};

inline HttpInfo::HttpInfo(HttpVersion version, size_t contentLength,
                          const HeaderFieldList& headers)
    : version_(version),
      contentLength_(contentLength),
      headers_(headers) {
  //.
}

}  // namespace xzero

