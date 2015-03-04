// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/logging/LogSource.h>
#include <xzero-base/logging/LogTarget.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/RuntimeError.h>
#include <xzero-base/StackTrace.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdarg.h>

namespace xzero {

LogSource::LogSource(const std::string& componentName)
    : componentName_(componentName) {
}

LogSource::~LogSource() {
}

#define LOG_SOURCE_MSG(level, fmt) \
  if (static_cast<int>(LogLevel::level) <=                                     \
      static_cast<int>(LogAggregator::get().logLevel())) {                     \
    if (LogTarget* target = LogAggregator::get().logTarget()) {                \
      char msg[512];                                                           \
      int n = snprintf(msg, sizeof(msg), "[%s] ", componentName_.c_str());     \
      va_list va;                                                              \
      va_start(va, fmt);                                                       \
      vsnprintf(msg + n, sizeof(msg) - n, fmt, va);                            \
      va_end(va);                                                              \
      target->level(msg);                                                      \
    }                                                                          \
  }

void LogSource::trace(const char* fmt, ...) {
  LOG_SOURCE_MSG(trace, fmt);
}

void LogSource::debug(const char* fmt, ...) {
  LOG_SOURCE_MSG(debug, fmt);
}

void LogSource::debug(const std::exception& e) {
  if (LogTarget* target = LogAggregator::get().logTarget()) {
    std::stringstream sstr;

    if (auto rt = dynamic_cast<const RuntimeError*>(&e)) {
      auto bt = rt->backtrace();

      sstr << "Unhandled exception caught ["
           << rt->sourceFile() << ':' << rt->sourceLine() << "] ("
           << StackTrace::demangleSymbol(typeid(e).name()).c_str()
           << "). " << rt->what() << std::endl;

      for (size_t i = 0; i < bt.size(); ++i)
        sstr << "  [" << i << "] " << bt[i] << std::endl;
    } else {
      sstr << "Unhandled exception caught in executor ("
           << StackTrace::demangleSymbol(typeid(e).name())
           << "): " << e.what();
    }

    target->debug(sstr.str());
  }
}

void LogSource::info(const char* fmt, ...) {
  LOG_SOURCE_MSG(info, fmt);
}

void LogSource::warn(const char* fmt, ...) {
  LOG_SOURCE_MSG(warn, fmt);
}

void LogSource::error(const char* fmt, ...) {
  LOG_SOURCE_MSG(error, fmt);
}

void LogSource::enable() {
  enabled_ = true;
}

bool LogSource::isEnabled() const XZERO_NOEXCEPT {
  return enabled_;
}

void LogSource::disable() {
  enabled_ = false;
}

}  // namespace xzero
