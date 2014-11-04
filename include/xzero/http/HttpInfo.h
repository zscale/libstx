// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

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
           const HeaderFieldList& headers,
           const HeaderFieldList& trailers);

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

  /** Tests whether HTTP message will send trailers. */
  bool hasTrailers() const noexcept { return !trailers_.empty(); }

  /** Retrieves the HTTP response trailers. */
  const HeaderFieldList& trailers() const noexcept { return trailers_; }

  void setTrailers(const HeaderFieldList& list) { trailers_ = list; }

 protected:
  HttpVersion version_;
  size_t contentLength_;
  HeaderFieldList headers_;
  HeaderFieldList trailers_;
};

inline HttpInfo::HttpInfo(HttpVersion version, size_t contentLength,
                          const HeaderFieldList& headers,
                          const HeaderFieldList& trailers)
    : version_(version),
      contentLength_(contentLength),
      headers_(headers),
      trailers_(trailers) {
  //.
}

}  // namespace xzero

