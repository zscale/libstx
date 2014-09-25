#pragma once

#include <xzero/Api.h>
#include <xzero/http/HttpInfo.h>
#include <string>

namespace xzero {

/**
 * HTTP Response Message Info.
 */
class XZERO_API HttpResponseInfo : public HttpInfo {
 public:
  HttpResponseInfo();

  HttpResponseInfo(HttpVersion version, HttpStatus status,
                   const std::string& reason, bool isHeadResponse,
                   size_t contentLength, const HeaderFieldList& headers);

  /** Retrieves the HTTP response status code. */
  HttpStatus status() const noexcept { return status_; }

  const std::string& reason() const noexcept { return reason_; }

  /** Retrieves whether this is an HTTP response to a HEAD request. */
  bool isHeadResponse() const noexcept { return isHeadResponse_; }

 private:
  HttpStatus status_;
  std::string reason_;
  bool isHeadResponse_;
};

inline HttpResponseInfo::HttpResponseInfo()
    : HttpResponseInfo(HttpVersion::UNKNOWN, HttpStatus::Undefined, "", false,
                       0, {}) {
}

inline HttpResponseInfo::HttpResponseInfo(HttpVersion version,
                                          HttpStatus status,
                                          const std::string& reason,
                                          bool isHeadResponse,
                                          size_t contentLength,
                                          const HeaderFieldList& headers)
    : HttpInfo(version, contentLength, headers),
      status_(status),
      reason_(reason),
      isHeadResponse_(isHeadResponse) {
  //.
}

}  // namespace xzero
