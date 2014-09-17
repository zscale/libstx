#pragma once

#include <xzero/Api.h>
#include <xzero/executor/LoopExecutor.h>
#include <deque>

namespace xzero {

/**
 * Basic Loop-Executor implementation.
 */
class XZERO_API DirectLoopExecutor : public LoopExecutor {
 public:
  DirectLoopExecutor();
  ~DirectLoopExecutor();

  // overrides
  void run() override;
  void cancel() override;
  void execute(Task&& task) override;
  size_t maxConcurrency() const noexcept override;
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


