#pragma once

#include <xzero/Api.h>
#include <xzero/TimeSpan.h>
#include <xzero/executor/Executor.h>
#include <functional>

namespace xzero {

/**
 * Interface for scheduling (possibly delayed) tasks.
 */
class XZERO_API Scheduler : public Executor {
 public:
  /**
   * Schedules given task to be run after given delay.
   *
   * @param task the actual task to be executed.
   * @param delay the timespan to wait before actually executing the task.
   */
  virtual void schedule(TimeSpan delay, Task&& task) = 0;
};

}  // namespace xzero
