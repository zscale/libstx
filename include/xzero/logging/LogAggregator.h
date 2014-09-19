#pragma once

#include <xzero/Api.h>
#include <xzero/logging/LogLevel.h>
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

  LogLevel logLevel() const noexcept { return logLevel_; }
  void setLogLevel(LogLevel level) { logLevel_ = level; }

  LogTarget* logTarget() const noexcept { return target_; }
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
