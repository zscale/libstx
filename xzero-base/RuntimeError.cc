// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

#include <xzero-base/RuntimeError.h>
#include <xzero-base/StackTrace.h>
#include <xzero-base/Tokenizer.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/StringUtil.h>
#include <xzero-base/logging.h>
#include <xzero-base/sysconfig.h>

#include <iostream>
#include <typeinfo>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#if defined(HAVE_EXECINFO_H)
#include <execinfo.h>
#endif

namespace xzero {

#define MAX_FRAMES 64
#define SKIP_FRAMES 2

void logAndPass(const std::exception& e) {
  logError("unknown", e);
}

void logAndAbort(const std::exception& e) {
  logAndPass(e);
  abort();
}

RuntimeError::RuntimeError(int ev, const std::error_category& ec)
  : std::system_error(ev, ec),
    sourceFile_(""),
    sourceLine_(0),
    functionName_(""),
    typeName_(nullptr),
    stackTrace_() {
}

RuntimeError::RuntimeError(
    int ev,
    const std::error_category& ec,
    const std::string& what)
  : std::system_error(ev, ec, what),
    sourceFile_(""),
    sourceLine_(0),
    functionName_(""),
    typeName_(nullptr),
    stackTrace_() {
}

RuntimeError::RuntimeError(Status ev)
  : RuntimeError((int) ev, StatusCategory::get()) {
}

RuntimeError::RuntimeError(Status ev, const std::string& what)
  : RuntimeError((int) ev, StatusCategory::get(), what) {
}

RuntimeError::~RuntimeError() {
}

RuntimeError& RuntimeError::setSource(const char* file, int line, const char* fn) {
  sourceFile_ = file;
  sourceLine_ = line;
  functionName_ = fn;
  return *this;
}

std::vector<std::string> RuntimeError::backtrace() const {
  return stackTrace_.symbols();
}

const char* RuntimeError::typeName() const {
  if (!typeName_) {
    typeName_ = typeid(*this).name();
  }
  return typeName_;
}

RuntimeError& RuntimeError::setTypeName(const char* n) {
  typeName_ = n;
  return *this;
}

bool RuntimeError::ofType(const char* s) const {
  return strcmp(typeName(), s) == 0;
}

void RuntimeError::debugPrint(std::ostream* os) const {
  if (os == nullptr) {
    os = &std::cerr;
  }

  *os << StringUtil::format(
            "$0: $1\n"
            "    in $2\n"
            "    in $3:$4\n",
            typeName(),
            what(),
            functionName_,
            sourceFile_,
            sourceLine_);
}

std::string RuntimeError::cformat(const char* fmt, ...) {
  va_list ap;
  char buf[1024];

  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  return std::string(buf, n);
}

} // namespace xzero
