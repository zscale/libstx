#pragma once

#include <xzero/Api.h>
#include <xzero/executor/Executor.h>

namespace xzero {

/**
 * Loop-Executor, that runs all enqueued tasks within the same loop.
 */
class XZERO_API LoopExecutor : public Executor {
 public:
  /**
   * Blocks the current thread until cancel() is invoked and runs all tasks 
   * that get scheduled.
   */
  virtual void run() = 0;

  /**
   * Cancels the currently active run() call and forces it to return to its
   * caller.
   */
  virtual void cancel() = 0;
};

} // namespace xzero

