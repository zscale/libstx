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

// a set of string constants to be used for exception type faking
const char kBufferOverflowError[] = "BufferOverflowError";
const char kEncodingError[] = "EncodingError";
const char kConcurrentModificationError[] = "ConcurrentModificationError";
const char kDivideByZeroError[] = "DivideByZeroError";
const char kFlagError[] = "FlagError";
const char kIOError[] = "IOError";
const char kIllegalArgumentError[] = "IOIllegalArgument";
const char kIllegalFormatError[] = "IllegalFormat";
const char kIllegalStateError[] = "IllegalState";
const char kIndexError[] = "IndexError";
const char kInvalidOptionError[] = "InvalidOptionError";
const char kKeyError[] = "KeyError";
const char kMallocError[] = "MallocError";
const char kNoSuchMethodError[] = "NoSuchMethodError";
const char kNotImplementedError[] = "NotImplementedError";
const char kNotYetImplementedError[] = "NotYetImplementedError";
const char kNullPointerError[] = "NullPointerError";
const char kParseError[] = "ParseError";
const char kRangeError[] = "RangeError";
const char kReflectionError[] = "kReflectionError";
const char kResolveError[] = "kResolveError";
const char kRPCError[] = "kRPCError";
const char kRuntimeError[] = "RuntimeError";
const char kTypeError[] = "TypeError";
const char kUsageError[] = "UsageError";
const char kVersionMismatchError[] = "VersionMismatchError";
const char kWouldBlockError[] = "WouldBlockError";
const char kFutureError[] = "FutureError";

class XZERO_API RuntimeError : public std::runtime_error {
 public:
  explicit RuntimeError(const std::string& what);
  ~RuntimeError();

  void setSource(const char* file, int line, const char* fn);
  const char* sourceFile() const { return sourceFile_; }
  int sourceLine() const XZERO_NOEXCEPT { return sourceLine_; }
  const char* functionName() const { return functionName_; }

  const char* typeName() const;
  void setTypeName(const char* n);
  bool ofType(const char* s) const;

  std::vector<std::string> backtrace() const;

 private:
  const char* sourceFile_;
  int sourceLine_;
  const char* functionName_;
  mutable const char* typeName_;
  StackTrace stackTrace_;
};

XZERO_API void logAndPass(const std::exception& e);
XZERO_API void logAndAbort(const std::exception& e);

/**
 * Helper function for the EXCEPTION(E, ...) macro.
 *
 * @internal
 *
 * @see EXCEPTION(E, ...)
 */
template<typename E, typename... Args>
inline E make_error(const char* file, int line, const char* fn, Args... args) {
  E e(args...);
  e.setSource(file, line, fn);
  return e;
}

} // namespace xzero

#define EXCEPTION(E, ...)                                                     \
  (make_error<E>(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__))

/**
 * Raises an exception of given type and arguments being passed to the
 * constructor.
 *
 * The exception must derive from RuntimeError, whose additional
 * source information will be initialized after.
 */
#define RAISE_EXCEPTION(E, ...) {                                             \
  E e(__VA_ARGS__);                                                           \
  e.setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__);                       \
  throw e;                                                                    \
}

/**
 * Raises an exception of given exception type.
 *
 * Alias to RAISE_EXCEPTION.
 */
#define RAISE(E, ...) RAISE_EXCEPTION(E, __VA_ARGS__)

/**
 * Raises a fake-typed RuntimeError exception.
 *
 * A RuntimeError exception is created and raised but its typeName
 * is customly set to the FakeType argument.
 */
#define RAISE_FAKE(FakeType, ...) {                                           \
  auto e = EXCEPTION(RuntimeError, __VA_ARGS__);                              \
  e.setTypeName(FakeType);                                                    \
  throw e;                                                                    \
}

/**
 * Raises an exception of given operating system error code.
 *
 * @see RAISE_EXCEPTION(E, ...)
 */
#define RAISE_ERRNO(errno) {                                                  \
  char buf[256];                                                              \
  strerror_r((errno), buf, sizeof(buf));                                      \
  size_t n = strlen(buf);                                                     \
  RAISE_EXCEPTION(RuntimeError, std::string(buf, n));                         \
}

/**
 * Raises an exception on given evaluated expression conditional.
 */
#define BUG_ON(cond) {                                                    \
  if (unlikely(cond)) {                                                   \
    RAISE_EXCEPTION(RuntimeError, "BUG ON: (" #cond ")");                 \
  }                                                                       \
}
