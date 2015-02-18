// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/sysconfig.h>
#include <xzero-base/logging/LogLevel.h>
#include <string>
#include <unordered_map>

namespace xzero {

class LogSource;
class LogTarget;

/**
 * Logging Aggregator Service.
 */
class XZERO_API LogAggregator {
 public:
  LogAggregator() : LogAggregator(LogLevel::Warning, nullptr) {}
  LogAggregator(LogLevel logLevel, LogTarget* logTarget);

  LogLevel logLevel() const XZERO_NOEXCEPT { return logLevel_; }
  void setLogLevel(LogLevel level) { logLevel_ = level; }

  LogTarget* logTarget() const XZERO_NOEXCEPT { return target_; }
  void setLogTarget(LogTarget* target);

  void registerSource(LogSource* source);
  void unregisterSource(LogSource* source);
  LogSource* findSource(const std::string& className) const;

  static LogAggregator& get();

 private:
  LogTarget* target_;
  LogLevel logLevel_;
  std::unordered_map<std::string, bool> enabledSources_;
  std::unordered_map<std::string, LogSource*> activeSources_;
};

}  // namespace xzero
