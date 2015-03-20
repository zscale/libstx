// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/Status.h>
#include <xzero-base/StackTrace.h>
#include <xzero-base/sysconfig.h>
#include <iosfwd>
#include <vector>
#include <string>
#include <stdexcept>
#include <system_error>
#include <string.h>
#include <errno.h>

namespace xzero {

class XZERO_API RuntimeError : public std::system_error {
 public:
  RuntimeError(int ev, const std::error_category& ec);
  RuntimeError(int ev, const std::error_category& ec, const std::string& what);

  explicit RuntimeError(Status ev);
  RuntimeError(Status ev, const std::string& what);

  template<typename... Args>
  RuntimeError(Status ev, const char* fmt, Args... args);

  ~RuntimeError();

  RuntimeError& setSource(const char* file, int line, const char* fn);
  const char* sourceFile() const { return sourceFile_; }
  int sourceLine() const XZERO_NOEXCEPT { return sourceLine_; }
  const char* functionName() const { return functionName_; }

  const char* typeName() const;
  RuntimeError& setTypeName(const char* n);
  bool ofType(const char* s) const;

  std::vector<std::string> backtrace() const;

  void debugPrint(std::ostream* os = nullptr) const;

  static std::string cformat(const char* fmt, ...);

 private:
  const char* sourceFile_;
  int sourceLine_;
  const char* functionName_;
  mutable const char* typeName_;
  StackTrace stackTrace_;
};

XZERO_API void logAndPass(const std::exception& e);
XZERO_API void logAndAbort(const std::exception& e);

// {{{ inlines
template<typename... Args>
inline RuntimeError::RuntimeError(
    Status ev,
    const char* fmt,
    Args... args)
  : RuntimeError((int) ev,
                 StatusCategory::get(),
                 cformat(fmt, args...)) {
}
// }}}

} // namespace xzero

#define EXCEPTION(E, ...)                                                     \
  (E(__VA_ARGS__).setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__))

/**
 * Raises an exception of given type and arguments being passed to the
 * constructor.
 *
 * The exception must derive from RuntimeError, whose additional
 * source information will be initialized after.
 */
#define RAISE_EXCEPTION(E, ...) {                                             \
  throw E(__VA_ARGS__).setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__);  \
}

//  throw (RuntimeError(Status:: __VA_ARGS__).setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__));

/**
 * Raises an exception of type RuntimeError for given code and category.
 *
 * Also sets source file, line, and calling function.
 *
 * @param code 1st argument is the error value.
 * @param category 2nd argument is the error category.
 * @param what_arg 3rd (optional) argument is the actual error message.
 */
#define RAISE_CATEGORY(Code, ...) {                                           \
  RAISE_EXCEPTION(RuntimeError, ((int) Code), __VA_ARGS__);                   \
}

/**
 * Raises an exception of given operating system error code.
 *
 * @see RAISE_EXCEPTION(E, ...)
 */
#define RAISE_ERRNO(errno) {                                                  \
  RAISE_CATEGORY(errno, std::system_category());                              \
}

/**
 * Raises a RuntimeError for error codes of type Status.
 *
 * @param StatusCode must be a member field of Status.
 */
#define RAISE_STATUS(StatusCode)                                              \
  RAISE_CATEGORY((Status:: StatusCode), StatusCategory::get())

/**
 * Alias to RAISE_STATUS(StatusCode).
 *
 * @param StatusCode must be a member of the enum class Status.
 */
#define RAISE(...) {                                                          \
  throw (RuntimeError(Status:: __VA_ARGS__).setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__));  \
}
  //RAISE_EXCEPTION(RuntimeError, (int) Status:: __VA_ARGS__);

/**
 * Raises an exception on given evaluated expression conditional.
 */
#define BUG_ON(cond) {                                                        \
  if (unlikely(cond)) {                                                       \
    RAISE_EXCEPTION(RuntimeError,                                             \
        (int) Status::InternalError,                                          \
        StatusCategory::get(),                                                \
        "BUG ON: (" #cond ")");                                               \
  }                                                                           \
}
