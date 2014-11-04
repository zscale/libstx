// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero/Api.h>
#include <stdexcept>

namespace xzero {

/**
 * Helper exception that is thrown on semantic message errors by HttpChannel.
 */
class XZERO_API BadMessage : public std::runtime_error {
 public:
  explicit BadMessage(HttpStatus code)
      : BadMessage(code, to_string(code)) {}

  explicit BadMessage(HttpStatus code, const std::string& reason)
      : std::runtime_error(reason),
        code_(code) {}

  HttpStatus code() const noexcept { return code_; }

 private:
  HttpStatus code_;
};

} // namespace xzero
