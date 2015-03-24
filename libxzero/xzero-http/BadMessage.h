// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-http/Api.h>
#include <xzero-http/HttpStatus.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/sysconfig.h>
#include <system_error>

namespace xzero {

class XZERO_HTTP_API HttpStatusCategory : public std::error_category {
 public:
  static std::error_category& get();

  const char* name() const noexcept;
  std::string message(int ev) const override;
};

/**
 * Helper exception that is thrown on semantic message errors by HttpChannel.
 */
class XZERO_HTTP_API BadMessage : public RuntimeError {
 public:
  explicit BadMessage(HttpStatus code);
  BadMessage(HttpStatus code, const std::string& reason);

  HttpStatus httpCode() const XZERO_NOEXCEPT {
    return static_cast<HttpStatus>(code().value());
  }
};

#define RAISE_HTTP(status) RAISE_EXCEPTION(BadMessage, (HttpStatus:: status))

} // namespace xzero
