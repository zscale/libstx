// This file is part of the "x0" project
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/RuntimeError.h>
#include <xzero-base/StackTrace.h>
#include <xzero-base/Tokenizer.h>
#include <xzero-base/Buffer.h>
#include <xzero-base/sysconfig.h>
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

#if defined(HAVE_DLFCN_H)
#include <dlfcn.h>
#endif

namespace xzero {

#define MAX_FRAMES 64
#define SKIP_FRAMES 2

void consoleLogger(const std::exception& e) { // {{{
  if (auto rt = dynamic_cast<const RuntimeError*>(&e)) {
    fprintf(stderr, "Unhandled exception caught [%s:%d] (%s). %s\n",
            rt->sourceFile(), rt->sourceLine(),
            StackTrace::demangleSymbol(typeid(e).name()).c_str(),
            rt->what());
    auto bt = rt->backtrace();
    for (size_t i = 0; i < bt.size(); ++i)
      fprintf(stderr, "  [%zu] %s\n", i, bt[i].c_str());
    return;
  }

  fprintf(stderr, "Unhandled exception caught in executor (%s): %s\n",
          StackTrace::demangleSymbol(typeid(e).name()).c_str(), e.what());
} // }}}

RuntimeError::RuntimeError(const std::string& what)
  : std::runtime_error(what),
    sourceFile_(""),
    sourceLine_(0),
    functionName_(""),
    typeName_(nullptr),
    stackTrace_() {
}

RuntimeError::~RuntimeError() {
}

void RuntimeError::setSource(const char* file, int line, const char* fn) {
  sourceFile_ = file;
  sourceLine_ = line;
  functionName_ = fn;
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

void RuntimeError::setTypeName(const char* n) {
  typeName_ = n;
}

bool RuntimeError::ofType(const char* s) const {
  return strcmp(typeName(), s) == 0;
}

} // namespace xzero
