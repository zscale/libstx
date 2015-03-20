// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License v3.0. You should have received a
// copy of the GNU General Public License along with this program. If not, see
// <http://www.gnu.org/licenses/>.

#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/TimeSpan.h>
#include <xzero-base/DateTime.h>
#include <xzero-base/executor/Executor.h>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>

namespace xzero {

/**
 * Interface for scheduling tasks.
 */
class XZERO_API Scheduler : public Executor {
 public:
  /**
   * It basically supports 2 actions, fire and cancellation.
   */
  class XZERO_API Handle {
   public:
    Handle(Task onFire, std::function<void(Handle*)> onCancel);

    /** Tests if the interest behind this handle was cancelled already. */
    bool isCancelled() const { return isCancelled_.load(); }

    /**
     * Cancels the interest, causing the callback not to be fired.
     */
    void cancel();

   private:
    /**
     * This method is invoked internally when the given intended event fired.
     *
     * Do not use this explicitely unless your code can deal with it.
     *
     * This method will not invoke the fire-callback if @c cancel() was invoked
     * already.
     */
    void fire();

    friend class Scheduler;

   private:
    std::mutex mutex_;
    Task onFire_;
    std::function<void(Handle*)> onCancel_;
    std::atomic<bool> isCancelled_;
  };

  typedef std::shared_ptr<Handle> HandleRef;

  Scheduler(std::function<void(const std::exception&)> eh)
      : Executor(std::move(eh)) {}

  /**
   * Schedules given task to be run after given delay.
   *
   * @param task the actual task to be executed.
   * @param delay the timespan to wait before actually executing the task.
   */
  virtual HandleRef executeAfter(TimeSpan delay, Task task) = 0;

  /**
   * Runs given task at given time.
   */
  virtual HandleRef executeAt(DateTime dt, Task task) = 0;

  /**
   * Runs given task when given selectable is non-blocking readable.
   */
  virtual HandleRef executeOnReadable(int fd, Task task) = 0;

  /**
   * Runs given task when given selectable is non-blocking writable.
   */
  virtual HandleRef executeOnWritable(int fd, Task task) = 0;

  /**
   * Retrieves the number of active timers.
   *
   * @see executeAt(DateTime dt, Task task)
   * @see executeAfter(TimeSpan ts, Task task)
   */
  virtual size_t timerCount() = 0;

  /**
   * Retrieves the number of active read-interests.
   *
   * @see executeOnReadable(int fd, Task task)
   */
  virtual size_t readerCount() = 0;

  /**
   * Retrieves the number of active write-interests.
   *
   * @see executeOnWritable(int fd, Task task)
   */
  virtual size_t writerCount() = 0;

  /**
   * Retrieves the number of pending tasks.
   *
   * @see execute(Task task)
   */
  virtual size_t taskCount() = 0;

  /**
   * Runs the event loop until no event is to be served.
   */
  virtual void runLoop() = 0;

  /**
   * Runs the event loop exactly once, possibly blocking until an event is
   * fired..
   */
  virtual void runLoopOnce() = 0;

  /**
   * Breaks loop in case it is blocking while waiting for an event.
   */
  virtual void breakLoop() = 0;

 protected:
  void safeCallEach(std::vector<HandleRef>& handles) {
    for (HandleRef& handle: handles)
      safeCall(std::bind(&Handle::fire, handle));
  }

  void safeCallEach(const std::deque<Task>& tasks) {
    for (Task task: tasks) {
      safeCall(task);
    }
  }

};

}  // namespace xzero
