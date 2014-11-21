// This file is part of the "x0" project, http://xzero.io/
//   (c) 2009-2014 Christian Parpart <trapni@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <xzero/Api.h>
#include <xzero/sysconfig.h>
#include <xzero/executor/LoopExecutor.h>
#include <deque>

namespace xzero {

/**
 * Basic Loop-Executor implementation.
 */
class XZERO_API DirectLoopExecutor : public LoopExecutor {
 public:
  DirectLoopExecutor() : DirectLoopExecutor(nullptr) {}
  DirectLoopExecutor(std::function<void(const std::exception&)>&& eh);
  ~DirectLoopExecutor();

  // overrides
  void run() override;
  void cancel() override;
  void execute(Task&& task) override;
  size_t maxConcurrency() const XZERO_NOEXCEPT override;
  std::string toString() const override;

  bool tryRunOne();
  bool tryRunAllPending();

 private:
  bool waitForEvent();
  void wakeup();

 private:
  std::deque<Task> pending_;
  bool isCancelled_;
  int pipe_[2];
};

} // namespace xzero


