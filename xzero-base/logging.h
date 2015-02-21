#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogLevel.h>
#include <xzero-base/logging/LogSource.h>

namespace xzero {

template<typename Component, typename Fmt, typename... Args>
XZERO_API void logError(Component component, Fmt fmt, Args... args) {
  LogAggregator::get().getSource(component)->error(fmt, args...);
}

template<typename Component, typename Fmt, typename... Args>
XZERO_API void logWarning(Component component, Fmt fmt, Args... args) {
  LogAggregator::get().getSource(component)->warn(fmt, args...);
}

template<typename Component, typename Fmt, typename... Args>
XZERO_API void logInfo(Component component, Fmt fmt, Args... args) {
  LogAggregator::get().getSource(component)->info(fmt, args...);
}

template<typename Component, typename Fmt, typename... Args>
XZERO_API void logDebug(Component component, Fmt fmt, Args... args) {
  LogAggregator::get().getSource(component)->debug(fmt, args...);
}

template<typename Component, typename Fmt, typename... Args>
XZERO_API void logTrace(Component component, Fmt fmt, Args... args) {
  LogAggregator::get().getSource(component)->trace(fmt, args...);
}

}  // namespace xzero
