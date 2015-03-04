// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/StackTrace.h>
#include <xzero-base/sysconfig.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <system_error>
#include <string.h>
#include <errno.h>

namespace xzero {

class XZERO_API RuntimeError : public std::runtime_error {
 public:
  explicit RuntimeError(const std::string& what);
  RuntimeError(const std::string& what, const char* sourceFile, int sourceLine);
  ~RuntimeError();

  void setSource(const char* file, int line);
  const char* sourceFile() const { return sourceFile_; }
  int sourceLine() const XZERO_NOEXCEPT { return sourceLine_; }

  std::vector<std::string> backtrace() const;

 private:
  const char* sourceFile_;
  int sourceLine_;
  StackTrace stackTrace_;
};

XZERO_API void consoleLogger(const std::exception& e);

} // namespace xzero

#define RAISE_EXCEPTION(E, ...)                                               \
    do {                                                                      \
      E e(__VA_ARGS__);                                                       \
      e.setSource(__FILE__, __LINE__);                                        \
      throw e;                                                                \
    } while (0)

#define RAISE(E, ...) RAISE_EXCEPTION(E, __VA_ARGS__)

#define RAISE_ERRNO() {                                                       \
  char buf[256];                                                              \
  strerror_r(errno, buf, sizeof(buf);                                         \
  size_t n = strlen(buf);                                                     \
  RAISE_EXCEPTION(RuntimeError, std::string(buf, n));                         \
}

// ----------------------------------------------------------------------------

#define RUNTIME_ERROR(msg) (::xzero::RuntimeError((msg), __FILE__, __LINE__))

#define SYSTEM_ERROR(errc) \
  (RUNTIME_ERROR(std::system_error(errc, std::system_category()).what()))

#if !defined(BUG_ON)
  #define BUG_ON(cond) {                                                    \
    if (unlikely(cond)) {                                                   \
      throw ::xzero::RuntimeError("BUG_ON: (" #cond ")",                    \
                                  __FILE__, __LINE__);                      \
    }                                                                       \
  }
#endif
