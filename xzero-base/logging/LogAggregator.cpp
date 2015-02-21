// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero-base/logging/LogAggregator.h>
#include <xzero-base/logging/LogSource.h>
#include <xzero-base/logging/LogTarget.h>
#include <stdlib.h>

namespace xzero {

LogAggregator::LogAggregator()
    : LogAggregator(LogLevel::Error, LogTarget::console()) {
}

LogAggregator::LogAggregator(LogLevel logLevel, LogTarget* logTarget)
    : target_(logTarget),
      logLevel_(logLevel),
      enabledSources_(),
      activeSources_() {
}

LogAggregator::~LogAggregator() {
  for (auto& activeSource: activeSources_)
    delete activeSource.second;

  activeSources_.clear();
}

void LogAggregator::setLogTarget(LogTarget* target) {
  target_ = target;
}

void LogAggregator::registerSource(LogSource* source) {
  std::lock_guard<std::mutex> _guard(mutex_);
  activeSources_[source->componentName()] = source;
}

void LogAggregator::unregisterSource(LogSource* source) {
  std::lock_guard<std::mutex> _guard(mutex_);
  auto i = activeSources_.find(source->componentName());
  activeSources_.erase(i);
}

LogSource* LogAggregator::getSource(const std::string& componentName) {
  std::lock_guard<std::mutex> _guard(mutex_);

  auto i = activeSources_.find(componentName);

  if (i != activeSources_.end())
    return i->second;
  else {
    LogSource* source = new LogSource(componentName);
    activeSources_[componentName] = source;
    return source;
  }
}

LogAggregator& LogAggregator::get() {
  static LogAggregator aggregator;
  return aggregator;
}

} // namespace xzero
