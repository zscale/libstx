#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogLevel.h>
#include <xzero-base/logging/LogSource.h>
#include <string>

namespace xzero {

template<typename... Args>
XZERO_API void logError(const char* component, const char* fmt, Args... args) {
  LogAggregator::get().getSource(component)->error(fmt, args...);
}

template<typename... Args>
XZERO_API void logWarning(const char* component, const char* fmt, Args... args) {
  LogAggregator::get().getSource(component)->warn(fmt, args...);
}

template<typename... Args>
XZERO_API void logInfo(const char* component, const char* fmt, Args... args) {
  LogAggregator::get().getSource(component)->info(fmt, args...);
}

template<typename... Args>
XZERO_API void logDebug(const char* component, const char* fmt, Args... args) {
  LogAggregator::get().getSource(component)->debug(fmt, args...);
}

template<typename... Args>
XZERO_API void logTrace(const char* component, const char* fmt, Args... args) {
  LogAggregator::get().getSource(component)->trace(fmt, args...);
}

XZERO_API void logError(const char* component, const std::exception& e);

}  // namespace xzero
